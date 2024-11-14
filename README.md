# HT12D emulator for PIC

## Introduction

This project was born from the need to decode the signals from sensors already installed in my house. 

The sensors transmit signals using the HT12E encoder and the receiver uses its HT12D pair to decode and process the signals. 

As I intend to redo the receiver circuit, I decided to try removing the HT12D and also a CD4093, replacing them with a single MCU PIC.

<img src="images/perf_windows_python.png" width="800"> 


## HT12E/HT12D Operation

Here, I don't want to exhaust the subject as there are many other websites/blogs that describe the functioning of this pair of circuits. 

I will limit myself to some important aspects for understanding the C code.

<img src="images/perf_windows_python.png" width="800"> 

## MCU Selection

## Software Description

## Operation modes


<img src="images/perf_windows_python.png" width="800"> 

## Performance

This example uses 'excel_to_pandas.py' to encapsulates library functions. 
To access library functions directly, see the code in 'excel_to_pandas.py'.
To a full functional example, see 'example.py'.


```Python
# Including library to our code:
import excel_to_dataframe.excel_to_pandas as etd   

# Test access to library printing version:
print(etd.version())

# Opening an Excel Workbook:
# Function 'open_excel' will return:
# 	 0 	- success;
#   -1 	- file not found or
#   -2 	- file found, but it isn't a Excel valid file.
ret = etd.open_excel(<excel_file_full_path>) 

# Loading data from worksheet:
# Function 'ws_to_df' will return:
# A Pandas dataframe if success or -1 otherwise
my_pandas_dataframe_1 = etd.ws_to_df(<excel_worksheet_name_1>) 
my_pandas_dataframe_2 = etd.ws_to_df(<excel_worksheet_name_2>) 

# Closing an Excel Workbook. This is mandatory to change from an Excel File to another.
etd.close_excel()

```

# Personal site:
http://www.nrbenergia.somee.com/  

or

https://nrbenergia.azurewebsites.net/

# More info:
https://www.linkedin.com/in/nelsonrossibittencourt/
