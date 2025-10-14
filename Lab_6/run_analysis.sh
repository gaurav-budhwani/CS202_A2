#!/bin/bash

# virtual Environment
echo "[SETUP] Creating Python virtual environment..."
python3 -m venv venv
echo "[SETUP] Activating virtual environment..."
source venv/bin/activate
echo "[SETUP] Virtual environment activated."

# tools installation
echo "[INSTALL] Installing Bandit and Semgrep via pip..."
pip install pandas bandit semgrep flake8 dlint flake8-json &> /dev/null
echo "[INSTALL] Bandit and Semgrep installed."

# clone repo
echo "[CLONE] Cloning large-scale open-source projects..."
# using --depth 1 to save space and time by getting only the latest commit
git clone --depth 1 https://github.com/django/django.git projects/django
git clone --depth 1 https://github.com/pandas-dev/pandas.git projects/pandas
git clone --depth 1 https://github.com/scikit-learn/scikit-learn.git projects/scikit-learn
echo "[CLONE] Repositories cloned successfully into the 'projects/' directory."

# vulnerability analysis tools
echo "[ANALYSIS] Starting vulnerability scans. This may take a significant amount of time..."

# define projects and tools
PROJECTS=("django" "pandas" "scikit-learn")
TOOLS=("bandit" "semgrep" "dlint")

# loop through each project and run each tool
for project in "${PROJECTS[@]}"; do
    echo "--- Analyzing Project: $project ---"
    PROJECT_PATH="projects/$project"

    # run Bandit
    echo "[ANALYSIS] Running Bandit on $project..."
    bandit -r "$PROJECT_PATH" -f json -o "results/${project}_bandit.json"
    echo "Bandit analysis for $project complete."

    # run Semgrep
    # we use the community-driven security ruleset: p/python
    echo "[ANALYSIS] Running Semgrep on $project..."
    semgrep scan --config "p/python" "$PROJECT_PATH" --json -o "results/${project}_semgrep.json"
    echo "Semgrep analysis for $project complete."

    # run Dlint
    echo "[ANALYSIS] Running Dlint on $project..."
    # we use flake8 to run dlint (--select=DUO) and format the output as JSON   
    flake8 --select=DUO --format=json "$PROJECT_PATH" > "results/${project}_dlint.json"
    echo "Dlint analysis for $project complete."    
done