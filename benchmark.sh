set -e

meson setup build
meson compile -C build

sizes=(256 512 1024 2048 4096 8192)
chances=(1 10 25 50 75 90 99)

rm -rf benchmarks
mkdir benchmarks

for size in ${sizes[@]}
do
    mkdir benchmarks/size_${size}
    for chance in ${chances[@]}
    do
        mkdir benchmarks/size_${size}/chance_${chance}
        for i in {1..10}
        do
            ./build/demo ${size} ${i} ${chance} > benchmarks/size_${size}/chance_${chance}/iteration_${i}.json
        done
    done
done