all:
	clang++ -O2 -g -std=c++11 -Weverything -Werror -Wno-c++98-compat -Wno-padded -o combiner -Ideps cyhair-combiner.cc cyhair-writer.cc cyhair-loader.cc
