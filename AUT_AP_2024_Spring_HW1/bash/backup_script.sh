#!/bin/bash

if [ "$#" -lt 2 ] || [ "$#" -gt 4 ]; then
    echo "Usage: $0 <source_path> <destination_path> [-c|--compression <compression_level>]"
    exit 1
fi

source_path=$1
destination_path=$2
compression_level=""
compression_level_value=1

if [ "$#" -eq 4 ]; then
    if [ "$3" = "-c" ] || [ "$3" = "--compression" ]; then
        compression_level=$3
        compression_level_value=$4
    else
        echo "Invalid option: $3"
        exit 1
    fi
fi

time1=$(date)
cur_date=$(date "+%Y-%m-%d")
directory_name=$(basename "$source_path")
backup_file_name="${cur_date}_${directory_name}.zip"

if [ -n "$compression_level" ]; then
    zip -r -"$compression_level_value" "$destination_path/$backup_file_name" "$source_path"
else
    zip -r "$destination_path/$backup_file_name" "$source_path"
fi

if [ $? -ne 0 ]; then
    echo "Backup failed"
    exit 1
fi

log_file="$destination_path/backup_log.txt"

echo "Backup Log Report" > "$log_file"
echo "Date: $cur_date" >> "$log_file"
echo "Time: $(date "+%H:%M:%S %Z")" >> "$log_file"
echo "Machine Information:" >> "$log_file"
echo "Operating System: $(uname -a)" >> "$log_file"
echo "Backup Details:" >> "$log_file"
echo "Source Path: $source_path" >> "$log_file"
echo "Destination Path: $destination_path/$backup_file_name" >> "$log_file"
echo "Compression Level: $compression_level_value" >> "$log_file"
echo "Files Backed Up: $(find "$source_path" -type f | wc -l)" >> "$log_file"
echo "Directories Backed Up: $(find "$source_path" -type d | wc -l)" >> "$log_file"
echo "Backup Summary:" >> "$log_file"
echo "Start Time: $time1" >> "$log_file"
echo "End Time: $(date)" >> "$log_file"
echo "Total Duration: $(($(date +%s) - $(date -d "$time1" +%s))) seconds" >> "$log_file"