import time
import sys
import matplotlib.pyplot as plt # https://matplotlib.org/
import numpy as np #
import ROOT
import os
import pandas
import array
import ftplib
from os import listdir
from os.path import isfile, join





ftp = ftplib.FTP("ufsdnas.local","ufsduser", "betascope")
print(ftp.getwelcome())
ftp.cwd("CENPA_TB_Aug2023/raw_analyzed_data/")

raw_root_list = ftp.nlst()
for f_in in raw_root_list:
    print("starting file " + f_in)
    ftp.retrbinary("RETR " + f_in, open(f_in, 'wb').write)
    os.system("./bin/CENPATest .  *.root")
    os.system("mv stats*.root /media/mnt/BigHD/CENPA_stat_files")
    os.system("rm *.root")
    print("ending " + f_in)
    
ftp.quit()
