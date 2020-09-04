FROM debian:testing
RUN apt-get update
RUN apt-get install -y socat
RUN apt-get install -y patchelf
WORKDIR /game
ADD dist/ .
EXPOSE 13372
RUN useradd -ms /bin/bash ctf

# ENV LD_LIBRARY_PATH=/game:$LD_LIBRARY_PATH

RUN chown ctf:ctf /game
RUN chown ctf:ctf /game/r2pm2
RUN chown ctf:ctf /game/flag.txt
RUN chmod 100 /game/r2pm2
RUN chmod 400 /game/flag.txt
USER ctf

CMD ["patchelf", "--set-interpreter", "/game/ld-2.31.so", "/game/r2pm2"]

# ENV LD_PRELOAD=/game/libc.so.6
ENV LD_LIBRARY_PATH=/game
CMD ["sh", "-c", "socat TCP-LISTEN:13372,reuseaddr,fork EXEC:\"/game/r2pm2\""]
