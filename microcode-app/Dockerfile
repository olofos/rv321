FROM node:10 AS node

FROM node as builder

RUN mkdir -p /build/client
COPY ./client/package.json /build/client

WORKDIR /build/client 
RUN npm install

COPY . /build
RUN npm run build

FROM node


WORKDIR /app
COPY --from=builder /build/server /app/
COPY --from=builder /build/client/build /app/public

ENV HTTP_PORT=3000
EXPOSE ${HTTP_PORT}/tcp
RUN npm install

ENTRYPOINT ["node", "index.js"]
