./build.sh || { echo 'build failed' ; exit 1; }
./upload.sh || { echo 'upload failed' ; exit 1; }