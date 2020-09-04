FROM debian:latest

WORKDIR /game
ADD bin/ . 
EXPOSE 7878
RUN useradd -ms /bin/bash ctf
RUN chown ctf:ctf /game
RUN chown ctf:ctf /game/r2ulette
RUN chown ctf:ctf /game/flag.txt
RUN chmod 100 /game/r2ulette
RUN chmod 400 /game/flag.txt
USER ctf
CMD /game/r2ulette
