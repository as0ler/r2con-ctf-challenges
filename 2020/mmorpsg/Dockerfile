FROM debian:latest
RUN apt-get update
RUN apt-get install -y socat
WORKDIR /game
ADD dist/ .
EXPOSE 13371
RUN useradd -ms /bin/bash ctf

RUN chown ctf:ctf /game
RUN chown ctf:ctf /game/mmorpsg
RUN chown ctf:ctf /game/flag.txt
RUN chmod 100 /game/mmorpsg
RUN chmod 400 /game/flag.txt
USER ctf

CMD ["sh", "-c", "socat TCP-LISTEN:13371,reuseaddr,fork EXEC:\"/game/mmorpsg\""]
