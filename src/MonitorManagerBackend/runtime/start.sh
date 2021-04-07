#!/usr/bin/bash
LD_LIBRARY_PATH=${pwd}
export LD_LIBRARY_PATH
nohup ./MonitorManager > log.out &
