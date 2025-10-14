## To execute this Lab

### Install the Required tools

``` brew install graphviz```
This tool is required by the pygraphviz library to render the graphs.

### Using a Virtual Environment
```
python3 -m venv venv
source venv/bin/activate
```
With the virtual environment active, install the required Python libraries using ```pip```.
```
pip install matplotlib pygraphviz
```

### The C Programs are located inside the ```c_programs``` Folder

### The analyzer is located at the root as ```analyzer.py```
In the project's root directory, give ```execute_all.sh``` script execution permissions.

```
chmod +x execute_all.sh
```

Execute the script to run the analysis on all three C programs.

```
./execute_all.sh
```
OR
In case of running the analysis on Individual C programs, use the following command after reaching inside the ```Lab_7``` folder
```
python analyzer.py c_programs/programi.c
```
