import os
import json
import csv
from collections import defaultdict
import pandas as pd

# multiple print statements for debugging purposes (to know if the program is working or not)
RESULTS_DIR = "results"
PROJECTS = ["django", "pandas", "scikit-learn"]
TOOLS = ["bandit", "semgrep", "dlint"]

# 2024 CWE Top 25 List
CWE_TOP_25 = {
    "CWE-79", "CWE-89", "CWE-416", "CWE-78", "CWE-787", "CWE-22", "CWE-352",
    "CWE-434", "CWE-287", "CWE-190", "CWE-125", "CWE-476", "CWE-20", "CWE-77",
    "CWE-502", "CWE-276", "CWE-862", "CWE-400", "CWE-918", "CWE-306", "CWE-362",
    "CWE-94", "CWE-269", "CWE-611", "CWE-119"
}

def parse_bandit_results(filepath):
    """Parses Bandit JSON output and extracts CWE IDs."""
    findings = defaultdict(int)
    try:
        with open(filepath, 'r') as f:
            data = json.load(f)
        for result in data.get("results", []):
            cwe_id_str = result.get("issue_cwe", {}).get("id")
            if cwe_id_str:
                # bandit sometimes gives multiple CWEs, e.g., 'CWE-20,CWE-79'
                cwe_ids = str(cwe_id_str).split(',')
                for cwe_id in cwe_ids:
                    findings[f"CWE-{cwe_id.strip()}"] += 1
    except (json.JSONDecodeError, FileNotFoundError):
        print(f"Warning: Could not parse or find {filepath}")
    return findings

def parse_semgrep_results(filepath):
    """Parses Semgrep JSON output and extracts CWE IDs."""
    findings = defaultdict(int)
    try:
        with open(filepath, 'r') as f:
            data = json.load(f)
        for result in data.get("results", []):
            cwe_list = result.get("extra", {}).get("metadata", {}).get("cwe", [])
            if isinstance(cwe_list, str): # Semgrep can return a string or list
                cwe_list = [cwe_list]
            for cwe in cwe_list:
                # format is often "CWE-ID: Name", so we extract the ID
                cwe_id = cwe.split(':')[0]
                findings[cwe_id.strip()] += 1
    except (json.JSONDecodeError, FileNotFoundError):
        print(f"Warning: Could not parse or find {filepath}")
    return findings

def parse_dlint_results(filepath):
    """Parses Dlint/Flake8 JSON output and maps DUO codes to CWE IDs."""
    findings = defaultdict(int)

    # Dlint's DUO codes are mapped to CWEs in its documentation.
    # we'll create a dictionary for the most common ones.
    dlint_to_cwe_map = {
        "DUO101": "CWE-22",   # Path Traversal
        "DUO104": "CWE-327",  # Broken Crypto
        "DUO105": "CWE-79",   # XSS
        "DUO106": "CWE-321",  # Hardcoded Key
        "DUO107": "CWE-89",   # SQL Injection
        "DUO110": "CWE-502",  # Insecure Deserialization
        "DUO113": "CWE-78",   # Command Injection
        "DUO131": "CWE-489",  # Hardcoded Password
    }

    try:
        with open(filepath, 'r') as f:
            data = json.load(f)
        # the JSON output is a dictionary where keys are filenames
        for filename in data:
            for error in data[filename]:
                duo_code = error.get("code")
                if duo_code in dlint_to_cwe_map:
                    cwe_id = dlint_to_cwe_map[duo_code]
                    findings[cwe_id] += 1
    except (json.JSONDecodeError, FileNotFoundError):
        print(f"Warning: Could not parse or find {filepath}")
    return findings

def main():
    """Main function to run the analysis."""
    print("\nRunning Analysis of Tool Results") # for debugging purpose
    
    # map tool names to their respective parser functions
    parsers = {
    "bandit": parse_bandit_results,
    "semgrep": parse_semgrep_results,
    "dlint": parse_dlint_results
    }
    # aggregate all findings into a consolidated list
    all_findings_list = []
    
    for project in PROJECTS:
        for tool in TOOLS:
            filepath = os.path.join(RESULTS_DIR, f"{project}_{tool}.json")
            cwe_counts = parsers[tool](filepath)
            
            for cwe_id, count in cwe_counts.items():
                is_top_25 = "Yes" if cwe_id in CWE_TOP_25 else "No"
                all_findings_list.append({
                    "Project_name": project,
                    "Tool_name": tool,
                    "CWE_ID": cwe_id,
                    "Number of Findings": count,
                    "Is_In_CWE_Top_25?": is_top_25
                })

    # write the consolidated data to a CSV file
    csv_file = "consolidated_results.csv"
    with open(csv_file, 'w', newline='') as f:
        writer = csv.DictWriter(f, fieldnames=all_findings_list[0].keys())
        writer.writeheader()
        writer.writerows(all_findings_list)
    print(f"[OUTPUT] Consolidated data - saved to '{csv_file}'.")

    # tool-level CWE Coverage analysis
    tool_cwe_sets = defaultdict(set)
    for finding in all_findings_list:
        tool_cwe_sets[finding["Tool_name"]].add(finding["CWE_ID"])

    print("\nTool-level CWE Coverage Analysis")
    print("Total Unique CWEs Detected per Tool:")
    for tool, cwes in tool_cwe_sets.items():
        print(f"  - {tool}: {len(cwes)} unique CWEs")

    print("\nTop 25 CWE Coverage (% of Top 25 found by each tool):")
    for tool, cwes in tool_cwe_sets.items():
        top_25_found = cwes.intersection(CWE_TOP_25)
        coverage_percentage = (len(top_25_found) / len(CWE_TOP_25)) * 100
        print(f"  - {tool}: Found {len(top_25_found)}/{len(CWE_TOP_25)} -> {coverage_percentage:.2f}% coverage.")

    # pairwise agreement (IoU) analysis
    iou_matrix = pd.DataFrame(index=TOOLS, columns=TOOLS, dtype=float)

    for tool1 in TOOLS:
        for tool2 in TOOLS:
            set1 = tool_cwe_sets[tool1]
            set2 = tool_cwe_sets[tool2]
            
            intersection = len(set1.intersection(set2))
            union = len(set1.union(set2))
            
            iou_score = intersection / union if union != 0 else 0
            iou_matrix.loc[tool1, tool2] = iou_score

    print("\nPairwise Agreement (IoU) Matrix")
    print("IoU (Jaccard Index) = |Intersection| / |Union|")
    print(iou_matrix.round(3))
    
    # save the matrix to a file for the report
    iou_matrix.to_csv("iou_matrix.csv")
    print("[OUTPUT] IoU matrix saved to 'iou_matrix.csv'.")

if __name__ == "__main__":
    main()


