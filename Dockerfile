
FROM archlinux

RUN pacman -Syu --noconfirm \
    && pacman -S --noconfirm git base-devel \
    && useradd -m builder \
    && echo "builder ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers \
    && su builder -c "git clone https://aur.archlinux.org/yay.git /tmp/yay" \
    && su builder -c "cd /tmp/yay && makepkg -si --noconfirm" \
    && rm -rf /tmp/yay \
    && pacman -Scc --noconfirm

RUN su builder -c "yay -S --noconfirm crow fmt spdlog asio"

WORKDIR /home/builder

COPY . .

RUN su builder -c "make clean && make"

CMD ["./keystore", "cli"]