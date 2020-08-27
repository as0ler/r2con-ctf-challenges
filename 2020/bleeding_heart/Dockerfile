FROM debian:latest
RUN apt-get update
RUN apt-get install -y socat
WORKDIR /game
ADD bin/ .
EXPOSE 1337
RUN useradd -ms /bin/bash ctf
RUN chown ctf:ctf /game
RUN chown ctf:ctf /game/bleeding_heart
RUN chown ctf:ctf /game/flag.txt
RUN chmod 100 /game/bleeding_heart
RUN chmod 400 /game/flag.txt
USER ctf
CMD ["sh", "-c", "socat TCP-LISTEN:1337,reuseaddr,fork EXEC:\"/game/bleeding_heart\""]
