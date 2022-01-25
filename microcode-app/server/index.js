const fs = require('fs');
const express = require('express');
const bodyParser = require('body-parser');
const addRequestId = require('express-request-id')({ setHeader: false });
const morgan = require('morgan');

const port = process.env.HTTP_PORT || 3001;

const app = express();

app.use(addRequestId);
morgan.token('id', (req) => req.id.split('-')[0]);

app.use(morgan('[:date[iso] #:id] Started :method :url for :remote-addr', { immediate: true }));
app.use(morgan('[:date[iso] #:id] Completed :status :res[content-length] in :response-time ms'));

app.use(bodyParser.json({limit: '50mb'}));

let data = JSON.parse(fs.readFileSync('./data/data.json'));

app.get('/api/data', (req, res) => {
    res.set('Access-Control-Allow-Origin', '*');
    res.send(data);
});

app.put('/api/data', (req, res) => {
    res.set('Access-Control-Allow-Origin', '*');
    data = req.body;
    fs.writeFileSync('./data/data.json', JSON.stringify(data,null,2));
    console.log('Datafile updated');
    res.status(204).end();
});



app.post('/api/exit', (req, res) => {
    res.set('Access-Control-Allow-Origin', '*');
    res.send('Exiting');

    console.log('Terminating process');
    process.kill(process.pid, 'SIGTERM');
});

app.use(express.static('public'));

const server = app.listen(port, () => {
    console.log(`Listening on port ${port}`);
});

process.on('SIGTERM', () => {
    console.log('Caught SIGTERM');
    
    server.close(() => {
      console.log('Process terminated')
    })
  });
