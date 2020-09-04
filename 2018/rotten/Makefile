all:
	GOOS=linux GOARCH=amd64 go build --ldflags "-w -s" -o bin/rotten src/rotten.go
	go build --ldflags "-w" -o bin/rotten_osx src/rotten.go

