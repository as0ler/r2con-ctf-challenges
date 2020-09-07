# Go in the Shell

*Level*: Easy

*Description*: Time to bust some GOsts...

*Flag*: See `flag.txt`

## Build

```
go build -ldflags="-s -w" ctf.go
```

## Deploy

```
docker build -t gointheshell .
docker run -d -p 3333:1337 gointheshell
```


