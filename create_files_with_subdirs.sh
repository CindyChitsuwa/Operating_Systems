# create main directory using date
dirname="project_$(date +"%Y-%m-%d-%H-%M-%S")"
mkdir "$dirname"

logfile="$dirname/script.log"
echo "[$(date +"%Y-%m-%d-%H-%M-%S")] Script started" >> "$logfile"

# languages to put in files
languages=("Python" "Java" "C" "JavaScript" "Go" "Ruby" "PHP" "Swift" "Kotlin" "Rust")

# create sub directories
for i in {0..9}; do
    folder_num=$((i + 101))
    folder="file$folder_num"
    mkdir "$dirname/$folder"

    echo "[$(date +"%Y-%m-%d-%H-%M-%S")] Created directory: $dirname/$folder" >> "$logfile"

    # create text files
    for j in {0..9}; do
        file_num=$((j + 501))
        file="tuser$file_num.txt"

        echo "${languages[$j]}" > "$dirname/$folder/$file"
        echo "[$(date +"%Y-%m-%d-%H-%M-%S")] Created file: $dirname/$folder/$file" >> "$logfile"
    done
done

echo "[$(date +"%Y-%m-%d-%H-%M-%S")] Script finished" >> "$logfile"