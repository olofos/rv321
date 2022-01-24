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

// function loadInstructionFragments(input) {
//     for (let fragmentName in input) {
//         const signalData = input[fragmentName].signals;
//         const empty_row = signalData['STEP_LEN'].values.map(() => '');

//         for (let signalName in signalData) {
//             const signal = signalData[signalName];
//             if (!signal.captions) {
//                 signal.captions = [...empty_row];
//             }
//         }
//     }
//     return input;
// }

// function loadSignals(input) {
//     for (let signalName in input) {
//         const signal = input[signalName];
//         if (signal.type === 'binary') {
//             signal.options = [0, 1];
//         }
//     }
//     return input;
// }


// function load_data()
// {
//     const instructions = JSON.parse(fs.readFileSync('./data/instructions.json'));
//     const instructionFragments = loadInstructionFragments(JSON.parse(fs.readFileSync('./data/instructionFragments.json')));
//     const signalSettings = loadSignals(JSON.parse(fs.readFileSync('./data/signals.json')));

//     return { instructions, fragments: instructionFragments, signals: signalSettings.signals, order: signalSettings.order };
// }

// let data = load_data();

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



app.listen(port, () => {
    console.log(`Listening on port ${port}`);
});
