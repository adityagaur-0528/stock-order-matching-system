FROM node:22-bookworm-slim

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY backend/package*.json ./backend/

RUN cd backend && npm ci --omit=dev

COPY . .

RUN g++ -std=c++17 -O2 \
    cpp/models.cpp \
    cpp/data_structures.cpp \
    cpp/trading_engine.cpp \
    cpp/engine_bridge.cpp \
    -o cpp/engine_bridge

RUN chmod +x cpp/engine_bridge

CMD ["npm", "--prefix", "backend", "start"]