make:
	g++ ./src/main.cpp -o sut -std=c++17

run:
	./main.exe

git_upload:
	-git add *
	-git commit
	-git push
