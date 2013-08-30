import utils.crash_binning as cbg
import os
#import copy.deepcopy

crash_dir = "crash-bins"

cb_total = cbg.crash_binning()

for cb_file in os.listdir(crash_dir):
    if(cb_file[-6:] != ".crash"):
        print("Not a crash file, skipping")
    cb_current = cbg.crash_binning()
    cb_current.import_file(crash_dir + "\\" + cb_file)
    cb_total.bins.update(cb_current.bins)
    cb_current = None

cb_total.export_file(crash_dir + "\\total.crash")
