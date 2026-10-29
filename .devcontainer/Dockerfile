ARG VARIANT=debian13
FROM mcr.microsoft.com/devcontainers/base:${VARIANT}

USER root

# Install needed packages
RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
    && apt-get -y install \
        build-essential \
        cmake \
        cppcheck \
        valgrind \
        clang-22 \
        lldb \
        llvm \
        gdb \
        g++-17 \
        curl \
        libcurl4t64 \
        zip \
        unzip \
        tar \
        pkg-config \
        ninja-build \
        googletest \
    && apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Setup environment variables for vcpkg
ENV VCPKG_ROOT=/usr/local/vcpkg \
    VCPKG_DOWNLOADS=/usr/local/vcpkg-downloads

ENV PATH="${PATH}:${VCPKG_ROOT}"

ARG USERNAME=vscode

# Install vcpkg
COPY base-scripts/install-vcpkg.sh /tmp/

RUN chmod +x /tmp/install-vcpkg.sh \
    && /tmp/install-vcpkg.sh ${USERNAME} \
    && rm -f /tmp/install-vcpkg.sh

# Fix Git ownership trust for vcpkg
RUN git config --global --add safe.directory /usr/local/vcpkg

# Install third-party C++ logging library
USER ${USERNAME}

RUN ${VCPKG_ROOT}/vcpkg install spdlog:arm64-linux

# Retrieve full Git history for versioned dependencies
RUN git -c safe.directory=/usr/local/vcpkg \
    -C /usr/local/vcpkg fetch --unshallow origin

# Trust the vcpkg repository for the vscode user
USER ${USERNAME}
RUN git config --global --add safe.directory /usr/local/vcpkg