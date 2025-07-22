FROM ubuntu:latest

WORKDIR /app

RUN apt-get update && apt-get install -y build-essential g++ make libspdlog-dev git wget libasio-dev

RUN wget https://github.com/CrowCpp/Crow/releases/download/v1.2.1.2/Crow-1.2.1-Linux.deb

RUN dpkg -i Crow-1.2.1-Linux.deb

COPY . .

RUN make

CMD ["./keystore", "https"]
