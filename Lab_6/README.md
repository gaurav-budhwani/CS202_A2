## To execute this Lab
### Create a virtual Environment
```
python3 -m venv venv
source venv/bin/activate
```
### Install the required tools that have been selected for this lab
```
pip install pandas bandit semgrep flake8 dlint flake8-json &> /dev/null
```

### clone the repositories inside the projects folder
```
git clone --depth 1 https://github.com/django/django.git projects/django
git clone --depth 1 https://github.com/pandas-dev/pandas.git projects/pandas
git clone --depth 1 https://github.com/scikit-learn/scikit-learn.git projects/scikit-learn
```

### vulnerability analysis tools
To further run the tools, a shell script has been written for convenience, since repeating a lot of statements consumes a lot of time.
The ```run_analysis.sh``` file automatically does everything described above as well as runs the tools and stores their output.

```
./run_analysis.sh
```

To run Individual Tools, the following commands can be used:
For bandit:
```
bandit -r "$PROJECT_PATH" -f json -o "results/${project}_bandit.json"
```

For Semgrep:
```
semgrep scan --config "p/python" "$PROJECT_PATH" --json -o "results/${project}_semgrep.json
```
For Dlint:
```
flake8 --select=DUO --format=json "$PROJECT_PATH" > "results/${project}_dlint.json
```
