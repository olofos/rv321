export function loadInstructionFragments(input) {
    for (let fragmentName in input) {
        const signalData = input[fragmentName].signals;
        const empty_row = signalData['STEP_LEN'].values.map(() => '');

        for (let signalName in signalData) {
            const signal = signalData[signalName];
            if (!signal.captions) {
                signal.captions = [...empty_row];
            }
        }
    }
    return input;
}

export function loadSignals(input) {
    for (let signalName in input) {
        const signal = input[signalName];
        if (signal.type === 'binary') {
            signal.options = [0, 1];
        }
    }
    return input;
}

function generateSyncronousSignals(signalData) {
    const generators = {
        'STEP': (index) => index,
        'LAST_STEP': (index) => (index === signalData['STEP_LEN'].values.length - 1) ? 1 : 0,
        'LENGTH_FIELD': (index) => {
            const bus_en = signalData['BUS_EN'].values[index];
            const step_len = signalData['STEP_LEN'].values[index];
            const last_step = index === signalData['STEP_LEN'].values.length - 1;

            if (bus_en) {
                if (step_len === 8) {
                    return 3;
                }
                return 2;
            }

            if (last_step) {
                return 0;
            }

            return 1;
        },
        'ALU_B_MUX': (index) => (signalData['IMM_MUX'].values[index] === 'ImmCSR') ? 'RS2' : signalData['IMM_MUX'].values[index],
        'PC_CNT_EN': (index) => (index === 0) ? 1 : 0,
        'IMM_PC_OUT_SP': (index) => {
            const busSteps = signalData['STEP_LEN'].values.slice(0, index + 1).filter((_, i) => signalData['BUS_EN'].values[i] === 1);
            const stepCount = busSteps.reduce((acc, val) => acc + val, 0);
            return ((signalData['BUS_EN'].values[index] === 1) || ((stepCount % 32) !== 0)) ? 1 : 0;
        },
    }

    const generatedSignals = {}
    for (let signal in generators) {
        generatedSignals[signal] = { values: [] };
        for (let index = 0; index < signalData['STEP_LEN'].values.length; index++) {
            generatedSignals[signal].values[index] = generators[signal](index);
        }
        generatedSignals[signal].parameters = generatedSignals[signal].values.map(() => '');
    }

    return generatedSignals;
}

function evaluateFragment(fragment, instructionFragments) {
    const { name, parameters } = fragment;
    const fragmentData = instructionFragments[name];
    const result = {}
    for (let signalName in fragmentData.signals) {
        const signal = fragmentData.signals[signalName];
        result[signalName] = {
            values: signal.values.map(value => parameters[value] ?? value),
            captions: signal.captions,
            parameters: signal.values.map(value => (value in parameters) ? value : ''),
        };
    }
    result['[fragment]'] = {
        values: fragmentData.signals['STEP_LEN'].values.map(() => name),
        captions: fragmentData.signals['STEP_LEN'].values.map(() => ''),
        parameters: fragmentData.signals['STEP_LEN'].values.map(() => ''),
    };
    return result;
}

function appendFragment(result, fragment) {
    for (let signalName in fragment) {
        if (result[signalName]) {
            result[signalName].values = result[signalName].values.concat(fragment[signalName].values);
            result[signalName].captions = result[signalName].captions.concat(fragment[signalName].captions);
            result[signalName].parameters = result[signalName].parameters.concat(fragment[signalName].parameters);
        } else {
            result[signalName] = {};
            result[signalName].values = fragment[signalName].values;
            result[signalName].captions = fragment[signalName].captions;
            result[signalName].parameters = fragment[signalName].parameters;
        }
    }
}

function addWarning(signalData, signalName, index, warning) {
    if (!signalData[signalName].warnings) {
        signalData[signalName].warnings = signalData[signalName].values.map(() => '');
    }
    if (signalData[signalName].warnings[index] !== '') {
        signalData[signalName].warnings[index] += '\n' + warning;
    } else {
        signalData[signalName].warnings[index] = warning;
    }
}

function addWarnings(signalData) {
    const lastIndex = signalData['STEP_LEN'].values.length - 1;
    const checks = [
        () => {
            if (signalData['STEP_LEN'].values[lastIndex] !== 1) {
                addWarning(signalData, 'LAST_STEP', lastIndex, 'The last step should have length 1.');
                addWarning(signalData, 'STEP_LEN', lastIndex, 'The last step should have length 1.');
            }
        },
        () => {
            const busSteps = signalData['STEP_LEN'].values.filter((_, index) => signalData['BUS_EN'].values[index] === 1);
            const stepCount = busSteps.reduce((acc, val) => acc + val, 0);

            if ((stepCount % 32) !== 0) {
                const index = signalData['BUS_EN'].values.findIndex((value) => value === 1);
                addWarning(signalData, 'BUS_EN', index, `There are ${stepCount} bus cycles, but number of bus cycles should be a multiple of 32.`)
            }
        },
        () => {
            if (signalData['BUS_EN'].values[lastIndex]) {
                addWarning(signalData, 'BUS_EN', lastIndex, 'The last step should have the bus disabled.');
                addWarning(signalData, 'LAST_STEP', lastIndex, 'The last step should have the bus disabled.');
            }
        },
        () => {
            let busVal = 0;
            let busCount = 0;
            for (let i = 0; i < lastIndex; i++) {
                if (signalData['BUS_EN'].values[i] === 1) {
                    busVal = 1;
                    busCount += signalData['STEP_LEN'].values[i];
                }
                if ((busVal === 1) && (signalData['BUS_EN'].values[i] === 0)) {
                    if ((busCount % 8) !== 0) {
                        addWarning(signalData, 'BUS_EN', i, 'The bus should be enabled in a multiple of eight cycles.');
                    }
                    busVal = 0;
                    busCount = 0;
                }
            }
        },
    ]
    checks.forEach((check) => check());
    return signalData;
}


export function assembleInstruction(instruction, instructions, instructionFragments) {
    const result = {};
    for (let fragment of instructions[instruction].fragments) {
        // const evaluatedFragment = evaluateFragment(instructionFragments[fragment.name], fragment.parameters);
        const evaluatedFragment = evaluateFragment(fragment, instructionFragments);
        // console.log(evaluatedFragment)
        appendFragment(result, evaluatedFragment);
    }

    const generatedSignals = generateSyncronousSignals(result);

    console.log(instruction, generatedSignals['STEP'].values.length)

    return addWarnings({ ...result, ...generatedSignals });
}

function opcodes(instruction, instructions) {
    const stepBits = 6;

    const opcodeData = instructions[instruction].opcode;
    let result = [0];

    [
        { key: 'op', max: 31, shift: 0 },
        { key: 'func3', max: 7, shift: 5 },
        { key: 'sys', max: 1, shift: 8 },
        { key: 'sa', max: 1, shift: 9 },
        { key: 'flag', max: 1, shift: 10 },
    ].forEach((field) => {
        if (field.key in opcodeData) {
            result = result.map(val => val + (opcodeData[field.key] << (field.shift + stepBits)));
        } else {
            let newResult = [];
            result.forEach((oldVal) => {
                for (let val = 0; val <= field.max; val++) {
                    newResult.push(oldVal + (val << (field.shift + stepBits)));
                }
            });
            result = newResult;
        }
    });

    return result;
}

function generateInstructionMicrocode(op, instructions, instructionFragments, signals) {
    const assembledInstruction = assembleInstruction(op, instructions, instructionFragments);
    const codes = Array(1 << 6).fill(0);
    const len = assembledInstruction['STEP_LEN'].values.length;
    for (let index = 0; index < len; index++) {
        codes[index] = Object.keys(assembledInstruction).map((key) =>
            signals[key].bitmap?.[assembledInstruction[key].values[index]] ?? 0
        ).reduce((acc, val) => acc + val, 0)
    }
    codes[len] = codes[len-1];
    return codes;
}


const cumulativeSum = (list, init = 0) => {
    const result = [];
    let sum = init;
    for (let i = 0; i < list.length; i++) {
        result[i] = sum;
        sum += list[i];
    }
    return result;
}

function generateClk(assembledInstruction) {
    const stepLengths = assembledInstruction['STEP_LEN'].values;
    const values = [{ time: -1, duration: 1, value: 1, parameter: '' }];
    let time = 0;
    for (let step of stepLengths) {
        for (let tick = 0; tick < step; tick++) {
            values.push({ time, duration: 0.5, value: 0, parameter: '' });
            values.push({ time: time + 0.5, duration: 0.5, value: 1, parameter: '' });
            time += 1;
        }
    }
    values.push({ time, duration: 1, value: 0, parameter: '' });
    return values;
}

function generateBusClk(assembledInstruction) {
    const stepLengths = assembledInstruction['STEP_LEN'].values;
    const busEn = assembledInstruction['BUS_EN'].values;
    const values = [{ time: -1, duration: 1, value: 0, parameter: '' }];
    let time = 0;

    for (let index = 0; index < stepLengths.length; index++) {
        if (busEn[index] === 1) {
            for (let tick = 0; tick < stepLengths[index]; tick++) {
                values.push({ time, duration: 0.5, value: 0, parameter: '' });
                values.push({ time: time + 0.5, duration: 0.5, value: 1, parameter: '' });
                time += 1;
            }
        } else {
            values.push({ time, duration: stepLengths[index], value: 0, parameter: '' });
            time += stepLengths[index];
        }
    }
    values.push({ time, duration: 1, value: 0, parameter: '' });
    return values;
}

function generateCount(assembledInstruction) {
    const stepLengths = assembledInstruction['STEP_LEN'].values;
    const values = [{ time: -1, duration: 1, value: '', parameter: '' }];
    let time = 0;
    for (let step of stepLengths) {
        for (let tick = 0; tick < step; tick++) {
            values.push({ time, duration: 1, value: time, parameter: '' });
            time += 1;
        }
    }
    values.push({ time, duration: 1, value: '', parameter: '' });
    return values;
}

export function generateInstructionTimeSeries(assembledInstruction, signals) {
    const stepLengths = [1, ...assembledInstruction['STEP_LEN'].values, 1];
    const stepTimes = cumulativeSum(stepLengths, -1);

    const outputSignals = {
        'CLK': generateClk(assembledInstruction),
        'BUS_CLK': generateBusClk(assembledInstruction),
        '[count]': generateCount(assembledInstruction),
    };

    for (let signalName in assembledInstruction) {
        let first = '';
        let last = '';

        if (signals[signalName].type === 'binary') {
            first = signals[signalName].pre;
            last = signals[signalName].post;
        }

        // console.log(signalName)
        const inputValues = [first, ...assembledInstruction[signalName].values, last];
        const inputParameters = ['', ...assembledInstruction[signalName].parameters, ''];
        const outputRow = []

        let prevValue = null;
        let prevParameter = null;

        for (let index = 0; index < inputValues.length; index++) {
            if ((!signals[signalName].noFusion) && (prevValue !== null) && (prevValue === inputValues[index]) && (prevParameter !== null) && (prevParameter === inputParameters[index])) {
                outputRow[outputRow.length - 1].duration += stepLengths[index];
            } else {
                outputRow.push({ time: stepTimes[index], duration: stepLengths[index], value: inputValues[index], parameter: inputParameters[index] })
                prevValue = inputValues[index];
                prevParameter = inputParameters[index];
            }
        }

        outputSignals[signalName] = outputRow;
    }
    return outputSignals;
}

// console.log(generateInstructionTimeSeries(assembleInstruction('BEQ [Taken]', instructions, instructionFragments), signals));

export function generateMicrocode(instructions, instructionFragments, signals) {
    const illop = generateInstructionMicrocode('IllOp', instructions, instructionFragments, signals);
    const microcode = Array(0x20000).map((_, index) => illop[index % 64]);
    Object.keys(instructions).forEach((op) => {
        const codes = generateInstructionMicrocode(op, instructions, instructionFragments, signals);

        opcodes(op, instructions).forEach((opcode) => {
            codes.forEach((code, index) => {
                microcode[opcode + index] = code;
            })
        });
        for (let opcodeVal = 0; opcodeVal < 0x800; opcodeVal++) {
            if (microcode[opcodeVal << 6] !== undefined) continue;
            for (let index = 0; index < 64; index++) {
                microcode[(opcodeVal << 6) + index] = microcode[(0x7FF << 6) + index];
            }
        }
    })
    return microcode;
}

export function formatDigitalHex(microcode) {
    return 'v2.0 raw\n' + microcode.map(n => n.toString(16).toUpperCase()).join('\n') + '\n';
}

// fs.writeFile('microcode.hex',formatDigitalHex(generateMicrocode(instructions, instructionFragments)), (err) => { if(err) { console.log(err); }});

export function generateGrid(assembledInstruction) {
    let time = 0;
    let prevFragment = null;
    let col = 0;
    const result = [];
    for (let index = 0; index < assembledInstruction['STEP_LEN'].values.length; index++) {
        const duration = assembledInstruction['STEP_LEN'].values[index];
        const fragment = assembledInstruction['[fragment]'].values[index];
        if (fragment !== prevFragment) {
            col = 0;
        }
        result.push({ time, duration, fragment, col });
        col += 1;
        prevFragment = fragment;
        time += duration;
    }
    return result;
}
