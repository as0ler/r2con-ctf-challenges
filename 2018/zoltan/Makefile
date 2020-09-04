all:
	go run src/dungeon.go > src/data.go
	go build --ldflags "-w" -o bin/zoltan_osx src/main.go src/data.go
	GOOS=linux GOARCH=386 go build --ldflags "-w" -o bin/zoltan src/main.go src/data.go

clean:
	rm -f bin/zoltan src/data.go
