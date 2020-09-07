FROM debian
RUN dpkg --add-architecture i386
RUN apt-get update
RUN apt-get install -y socat
RUN useradd --no-create-home --user-group ghost
WORKDIR /opt/
COPY ctf .
COPY flag.txt .
RUN chown root:root flag.txt
RUN chown root:root ctf
RUN chmod 444 flag.txt
RUN chmod 111 ctf
USER ghost
CMD ["sh", "-c", "socat TCP-LISTEN:1337,reuseaddr,fork EXEC:\"./ctf\""]