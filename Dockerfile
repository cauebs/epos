FROM ubuntu:jammy

WORKDIR /home

ENV TZ=America/Sao_Paulo
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ

RUN apt-get update && \
    apt-get install -y \
    autoconf \
    automake \
    autotools-dev \
    bc \
    bison \
    bin86 \
    build-essential \
    curl \
    flex \
    gawk \
    gcc-multilib \
    gdb \
    gdb-multiarch \
    gperf \
    g++-multilib \
    konsole \
    libexpat-dev \
    libgmp-dev \
    libmpc-dev \
    libmpfr-dev \
    libglib2.0-dev \
    libpixman-1-dev \
    libtool \
    make \
    patchutils \
    pkg-config \
    python3 \
    python3-pip \
    qemu \
    qemu-kvm \
    qemu-system-arm \
    qemu-system-misc \
    tar \
    tcsh \
    texinfo \
    unzip \
    wget \
    xterm \
    zlib1g-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN wget https://github.com/Yelp/dumb-init/releases/download/v1.2.2/dumb-init_1.2.2_amd64.deb && \
    dpkg -i dumb-init_*.deb && \
    rm dumb-init_*.deb

RUN wget -q -O ia32.tar.gz https://epos.lisha.ufsc.br/dl469 && \
    tar -zxvf ia32.tar.gz && \
    mkdir -p /usr/local/ia32/ && \
    mv gcc-7.2.0 /usr/local/ia32/ && \
    rm -r ia32.tar.gz

RUN wget -q -O arm.tar.gz https://epos.lisha.ufsc.br/dl468 && \
    tar -zxvf arm.tar.gz && \
    mkdir -p /usr/local/arm/ && \
    mv gcc-7.2.0 /usr/local/arm/ && \
    rm -r arm.tar.gz

RUN wget -q -O rv32.tar.gz https://www.dropbox.com/s/t7oiniijgq74a32/riscv.tar.gz?dl=1 && \
    tar -zxvf rv32.tar.gz && \
    mkdir -p /usr/local/rv32/ && \
    mv rv32/* /usr/local/rv32/ && \
    rm -r rv32.tar.gz

RUN wget --load-cookies /tmp/cookies.txt "https://docs.google.com/uc?export=download&confirm=$(wget --quiet --save-cookies /tmp/cookies.txt --keep-session-cookies --no-check-certificate 'https://docs.google.com/uc?export=download&id=16vQ7nO10FW1HVltXecGYr44Rqkb5gQcS' -O- | sed -rn 's/.*confirm=([0-9A-Za-z_]+).*/\1\n/p')&id=16vQ7nO10FW1HVltXecGYr44Rqkb5gQcS" -O rv64.zip && \
    rm -rf /tmp/cookies.txt && \
    unzip rv64.zip -d rv64 && \
    mkdir -p /usr/local/rv64/ && \
    mv rv64/* /usr/local/rv64/ && \
    rm -r rv64.zip

ENV PATH="${PATH}:/usr/local/arm/gcc-7.2.0/bin/"

VOLUME /code
WORKDIR /code

ENTRYPOINT ["/usr/bin/dumb-init", "--"]
