FROM debian:latest

WORKDIR /game
ADD bin/ .
EXPOSE 1337
RUN useradd -ms /bin/bash ctf
RUN chown ctf:ctf /game
RUN chown ctf:ctf /game/cerberus
RUN chown ctf:ctf /game/flag.txt
RUN chown ctf:ctf /game/.flag.txt
RUN chmod 100 /game/cerberus
RUN chmod 400 /game/flag.txt
RUN chmod 400 /game/.flag.txt
USER ctf
ENTRYPOINT /game/cerberus
