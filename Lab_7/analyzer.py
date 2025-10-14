import sys
import re
import os
import pygraphviz as pgv

def get_c_code_lines(filepath):
    """Reads a C file and returns a list of lines."""
    with open(filepath, 'r') as f:
        return [line.strip() for line in f.readlines() if line.strip()]

def find_leaders(lines):
    """Identifies leaders in the C code based on the lab rules."""
    leaders = {0}  # the first instruction is a leader
    for i, line in enumerate(lines):
        # target of a branch/jump/loop is a leader
        if re.match(r'^(if|while|for|switch)\s*\(', line) or re.match(r'^(else|case|default)', line):
            leaders.add(i)
        # target of a goto is also a leader
        if match := re.search(r'goto\s+(\w+);', line):
            label = match.group(1)
            for j, target_line in enumerate(lines):
                if re.match(rf'^{label}:', target_line):
                    leaders.add(j)
                    
        # instruction immediately after a branch/jump/loop is a leader
        if re.match(r'^(if|while|for|switch|goto|return|break)\b', line) or (line.startswith('}') and i > 0):
            if i + 1 < len(lines):
                # Avoid adding leaders for closing braces of a block
                if not lines[i+1].startswith('}'):
                    leaders.add(i + 1)
    
    # the line after an else block needs to be a leader too
    for i, line in enumerate(lines):
        if line.strip() == "} else {":
            if i + 1 < len(lines):
                leaders.add(i + 1)

    return sorted(list(leaders))

def create_basic_blocks(lines, leaders):
    """Groups lines of code into basic blocks using the identified leaders."""
    blocks = {}
    for i, start_index in enumerate(leaders):
        end_index = leaders[i+1] if i + 1 < len(leaders) else len(lines)
        block_code = lines[start_index:end_index]
        blocks[f"B{i}"] = {
            'code': block_code,
            'start_line': start_index,
            'end_line': end_index - 1
        }
    return blocks

def build_cfg(blocks):
    """Builds the Control Flow Graph by connecting basic blocks."""
    cfg = {block_id: [] for block_id in blocks}
    block_ids = list(blocks.keys())

    for i, block_id in enumerate(block_ids):
        last_line_of_block = blocks[block_id]['code'][-1]

        # conditional branches (if, while, for)
        if re.match(r'^(if|while|for)\s*\(', last_line_of_block):
            # true branch - typically the next block
            if i + 1 < len(block_ids):
                cfg[block_id].append(block_ids[i+1])

            # false branch - find the block that starts after the corresponding '}' or 'else'
            # this is a simplification. A real parser would build an Abstract Syntax Tree.
            current_block_end_line = blocks[block_id]['end_line']
            for j in range(i + 1, len(block_ids)):
                if "else" in blocks[block_ids[j]]['code'][0]:
                    cfg[block_id].append(block_ids[j])
                    break
            else: # If no else found, find the merge point
                 if i + 2 < len(block_ids):
                    cfg[block_id].append(block_ids[i+2])

        # unconditional jumps (else blocks jump to the merge point)
        elif "else" in blocks[block_id]['code'][0] or "case" in blocks[block_id]['code'][0]:
             if i + 2 < len(block_ids):
                 cfg[block_id].append(block_ids[i+2])
             elif i+1 < len(block_ids):
                 cfg[block_id].append(block_ids[i+1])


        # sequential flow
        else:
            if not any(kw in last_line_of_block for kw in ['return', 'break', 'goto']):
                if i + 1 < len(block_ids):
                    cfg[block_id].append(block_ids[i+1])
    
    # simple pass to clean up duplicate edges
    for block_id in cfg:
        cfg[block_id] = sorted(list(set(cfg[block_id])))
        
    return cfg

def visualize_cfg(cfg, blocks, filename):
    """Generates a .dot file and a PNG image for the CFG."""
    G = pgv.AGraph(directed=True)
    for block_id, block_info in blocks.items():
        label = f"{block_id}:\\n" + "\\l".join(block_info['code']) + "\\l"
        G.add_node(block_id, label=label, shape='box')

    for start_node, end_nodes in cfg.items():
        for end_node in end_nodes:
            G.add_edge(start_node, end_node)
    
    dot_path = f"{filename}.dot"
    png_path = f"{filename}.png"

    G.write(dot_path)
    G.layout(prog='dot')
    G.draw(png_path)
    print(f"CFG visualization saved to {png_path}")

# METRICS CALCULATION

def calculate_metrics(cfg):
    """Calculates N, E, and Cyclomatic Complexity."""
    num_nodes = len(cfg)
    num_edges = sum(len(edges) for edges in cfg.values())
    complexity = num_edges - num_nodes + 2
    return num_nodes, num_edges, complexity

# REACHING DEFINITIONS ANALYSIS

def find_definitions(blocks):
    """Finds all variable definitions (assignments) in the code."""
    definitions = {}
    def_count = 1
    var_to_defs = {}

    for block_id, block_info in blocks.items():
        for line in block_info['code']:
            # regex for simple assignments: var = ...; | var++; | var--;
            match = re.match(r'^\s*(\w+(\[\w+\])*(\[\w+\])*)\s*(=|\+\+|--|\+=|-=)', line)
            if match:
                var_name = match.group(1).split('[')[0] # Get base variable name
                def_id = f"d{def_count}"
                definitions[def_id] = {'var': var_name, 'block': block_id, 'line': line}
                
                if var_name not in var_to_defs:
                    var_to_defs[var_name] = set()
                var_to_defs[var_name].add(def_id)
                def_count += 1
                
    return definitions, var_to_defs

def compute_gen_kill(blocks, definitions, var_to_defs):
    """Computes the gen and kill sets for each basic block."""
    gen = {block_id: set() for block_id in blocks}
    kill = {block_id: set() for block_id in blocks}

    for def_id, def_info in definitions.items():
        block_id = def_info['block']
        var = def_info['var']

        # add to gen set of its own block
        gen[block_id].add(def_id)

        # add to kill sets of ALL blocks for that variable
        other_defs = var_to_defs[var] - {def_id}
        for b_id in blocks:
             # A block kills all other definitions of a variable it re-defines
             if def_id in gen[b_id]:
                kill[b_id].update(other_defs)

    return gen, kill
    
def get_predecessors(cfg):
    """Computes the predecessors for each block in the CFG."""
    predecessors = {block_id: [] for block_id in cfg}
    for node, edges in cfg.items():
        for edge in edges:
            predecessors[edge].append(node)
    return predecessors

def reaching_definitions_analysis(blocks, cfg, gen, kill):
    """Performs the iterative dataflow analysis."""
    in_sets = {block_id: set() for block_id in blocks}
    out_sets = {block_id: set() for block_id in blocks}
    predecessors = get_predecessors(cfg)

    changed = True
    iteration = 0
    history = []

    while changed:
        changed = False
        iteration += 1
        
        current_iter_out = {k: v.copy() for k, v in out_sets.items()}
        history.append(current_iter_out)
        
        print(f"\n--- Iteration {iteration} ---")
        print(f"{'Block':<10}{'in[B]':<30}{'out[B]':<30}")
        print("-" * 70)
        
        for block_id in blocks:
            # IN[B] = U OUT[P] for all predecessors P of B
            new_in = set()
            for p in predecessors[block_id]:
                new_in.update(out_sets[p])
            in_sets[block_id] = new_in

            # OUT[B] = gen[B] U (IN[B] - kill[B])
            old_out = out_sets[block_id]
            new_out = gen[block_id].union(in_sets[block_id] - kill[block_id])
            
            if new_out != old_out:
                changed = True
            out_sets[block_id] = new_out

            print(f"{block_id:<10}{str(sorted(list(in_sets[block_id]))):<30}{str(sorted(list(out_sets[block_id]))):<30}")

    print("\nConvergence reached!")
    return in_sets, out_sets

# MAIN EXECUTION

def main():
    if len(sys.argv) != 2:
        print("Usage: python analyzer.py <path_to_c_file>")
        sys.exit(1)

    c_filepath = sys.argv[1]
    if not os.path.exists(c_filepath):
        print(f"Error: File not found at {c_filepath}")
        sys.exit(1)
        
    base_filename = os.path.splitext(os.path.basename(c_filepath))[0]
    output_dir = "generated_cfgs"
    os.makedirs(output_dir, exist_ok=True)
    output_filepath = os.path.join(output_dir, base_filename)


    print(f"\n--- Analyzing {c_filepath} ---")

    lines = get_c_code_lines(c_filepath)
    leaders = find_leaders(lines)
    blocks = create_basic_blocks(lines, leaders)
    cfg = build_cfg(blocks)
    
    print("\n--- 1. Basic Blocks ---")
    for block_id, block_info in blocks.items():
        print(f"{block_id} (Lines {block_info['start_line']+1}-{block_info['end_line']+1}):")
        for line in block_info['code']:
            print(f"  {line}")

    visualize_cfg(cfg, blocks, output_filepath)

    print("\n--- 2. Cyclomatic Complexity Metrics ---")
    n, e, cc = calculate_metrics(cfg)
    print(f"Number of Nodes (N): {n}")
    print(f"Number of Edges (E): {e}")
    print(f"Cyclomatic Complexity (E - N + 2): {cc}")

    print("\n--- 3. Reaching Definitions Analysis ---")
    definitions, var_to_defs = find_definitions(blocks)
    
    print("\nIdentified Definitions:")
    for def_id, info in sorted(definitions.items(), key=lambda x: int(x[0][1:])):
        print(f"  {def_id}: {info['var']} (in {info['block']}, line: '{info['line']}')")

    gen, kill = compute_gen_kill(blocks, definitions, var_to_defs)
    
    print("\nGen/Kill Sets:")
    print(f"{'Block':<10}{'gen[B]':<30}{'kill[B]':<50}")
    print("-" * 90)
    for block_id in blocks:
        print(f"{block_id:<10}{str(sorted(list(gen[block_id]))):<30}{str(sorted(list(kill[block_id]))):<50}")
    
    in_sets, out_sets = reaching_definitions_analysis(blocks, cfg, gen, kill)
    
    print("\n--- Final Analysis Results ---")
    for block_id in blocks:
        print(f"At the entry of {block_id}, reaching definitions are: {sorted(list(in_sets[block_id]))}")

if __name__ == "__main__":
    main()

