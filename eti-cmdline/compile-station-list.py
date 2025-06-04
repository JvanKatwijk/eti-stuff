#! /usr/bin/python

# Takes a list of ensemble json files and compiles a
# master station-list.json

import os
import json

stations=dict()
obj = os.scandir()
for entry in obj :
    if entry.is_dir() or entry.is_file():
        if entry.name.startswith("ensemble-ch-") and entry.name.endswith(".json"):
            print(f'Processing {entry.name}')
            with open(entry.name, 'r') as jfile:
                data = json.load(jfile)
                for s,sid in data['stations'].items():
                    if s not in stations:
                        stations[s]={ 'sid':sid, 'ensemble':data['ensemble'], 'channel':data['channel'] }
                    else:
                        stations[s + " " + data['ensemble'] ]={ 'sid':sid, 'ensemble':data['ensemble'], 'channel':data['channel'] }
obj.close()
with open("station-list.json","w") as s:
    json.dump(stations,s)
print("Created station-list.json")

