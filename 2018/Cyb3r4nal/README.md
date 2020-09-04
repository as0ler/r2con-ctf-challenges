KR3ml1n Reversing Game
======

* bin: contains the binary compiled without being ofuscated
* ofuscated: containts the binary ofuscated (to send to the players)
* src: source of the game
* Dockerfile: Dockerfile to use to create the game
```
docker build --tag "ack-ctf/kr3ml1n:latest" .
docker run -d --name "kr3ml1n" -p 6969:6969 ack-ctf/kr3ml1n:latest
```

