import { createAsyncThunk, createSlice, createSelector } from '@reduxjs/toolkit';

import { loadInstructionFragments, loadSignals, assembleInstruction, generateInstructionTimeSeries, generateGrid, generateMicrocode, formatDigitalHex } from './transformations';

const initialState = {
    instructions: {},
    // instructions,
    instructionFragments: {},
    // instructionFragments: loadInstructionFragments(instructionFragmentsInput),
    settings: {},
    // settings: loadSignals(signalsInput.signals),
    order: [],
    // order: signalsInput.order,
    ui: {
        op: 'ADD',
        hideConstant: false,
        selection: {
            fragment: null,
            signal: null,
            columns: []
        }
    },
    status: 'starting'
};

export const loadData = createAsyncThunk(
    'timingDiagram/load',
    async () => {
        const response = await fetch('/api/data');
        return await response.json();
    }
)

export const saveData = createAsyncThunk(
    'timingDiagram/save',
    async ( unused , {getState}) => {
        const state = getState();
        await fetch('/api/data', {
            method: 'PUT', 
            headers: {'Content-Type': 'application/json'},
            body: JSON.stringify({
                instructions: state.timingDiagram.instructions,
                fragments: state.timingDiagram.instructionFragments,
                signals: state.timingDiagram.settings,
                order: state.timingDiagram.order,
            }),
        });
    }
)

const getSelectedSignalOptions = (state) => {
    const { signal, fragment } = state.ui.selection;
    if (!signal || !fragment || state.settings[signal].generated) return [];

    const parameters = state.instructionFragments[fragment].parameters;
    const parameter = Object.keys(parameters).filter(key => parameters[key].signal === signal);
    const options = [...state.settings[signal].options, ...parameter];

    return options;
}

const getSelectedSignalValues = (state) => {
    const { signal, fragment } = state.ui.selection;
    if (!signal || !fragment || state.settings[signal].generated) return [];
    const allValues = state.instructionFragments[fragment].signals[signal].values;
    const values = [];
    state.ui.selection.columns.forEach((col) => {
        if (!values.includes(allValues[col])) {
            values.push(allValues[col]);
        }
    })
    return values.sort();
}

export const timingDiagramSlice = createSlice({
    name: 'timingDiagram',
    initialState,
    reducers: {
        setParameterValue: (state, action) => {
            const { instruction, fragment, parameter, value } = action.payload;
            const options = state.instructionFragments[fragment].parameters[parameter].values;
            const fragmentEntry = state.instructions[instruction].fragments.find((frag) => frag.name === fragment);
            fragmentEntry.parameters[parameter] = options.find((val) => val.toString() === value.toString());
        },

        toggleSelectedSignalValues: (state) => {
            const { signal, fragment } = state.ui.selection;
            if (!signal || !fragment) return;
            if (state.settings[signal].generated) return;

            const values = state.instructionFragments[fragment].signals[signal].values;
            const options = getSelectedSignalOptions(state);

            state.ui.selection.columns.forEach((col) => {
                const oldValue = values[col];
                const oldIndex = options.findIndex(v => v.toString() === oldValue.toString());
                const newIndex = (oldIndex + 1) % options.length;
                values[col] = options[newIndex];
            });
        },

        setSelectedSignalValues: (state, action) => {
            const { signal, fragment } = state.ui.selection;
            if (state.settings[signal].generated) return;

            const values = state.instructionFragments[fragment].signals[signal].values;
            const options = getSelectedSignalOptions(state);
            const value = options.find(val => val.toString() === action.payload.toString());

            state.ui.selection.columns.forEach((col) => {
                values[col] = value;
            });
        },

        setCurrentInstruction: (state, action) => {
            state.ui.op = action.payload;
        },

        setHideConstant: (state, action) => {
            state.ui.hideConstant = action.payload;
        },

        setSelection: (state, action) => {
            const { fragment, signal } = action.payload;
            const columns = [action.payload.col];
            state.ui.selection = { fragment, signal, columns };
        },

        setFragmentSelection: (state, action) => {
            const fragment = action.payload;
            state.ui.selection = { fragment, signal: null, columns: [] };
        },

        setRowSelection: (state, action) => {
            const signal = action.payload;
            state.ui.selection = { fragment: null, signal, columns: [] };
        },

        setColumnSelection: (state, action) => {
            const { fragment } = action.payload;
            const columns = [action.payload.col];
            state.ui.selection = { fragment, signal: null, columns };
        },

        addSelection: (state, action) => {
            const { fragment, signal, columns } = action.payload;
            if ((fragment === state.ui.selection.fragment) && (signal === state.ui.selection.signal)) {

                const newColumns = columns.filter((col) => !state.ui.selection.columns.includes(col))
                // console.log([newColumns,[...state.ui.selection.columns, ...newColumns]])
                state.ui.selection.columns = [...state.ui.selection.columns, ...newColumns]
                // console.log([newColumns,[...state.ui.selection.columns]])
            } else {
                state.ui.selection = { fragment, signal, columns };
            }
        },

        removeSelection: (state, action) => {
            const { fragment, signal, columns } = action.payload;
            if ((fragment === state.ui.selection.fragment) && (signal === state.ui.selection.signal)) {
                if (columns.length === state.ui.selection.columns.length) {
                    state.ui.selection = { fragment: null, signal: null, columns: [] };
                } else {
                    state.ui.selection.columns = state.ui.selection.columns.filter((col) => !columns.includes(col));
                }
            } else {
                state.ui.selection = { fragment: null, signal: null, columns: [] };
            }
        },

        clearSelection: (state) => {
            state.ui.selection = { fragment: null, signal: null, columns: [] };
        },

        moveSelectedSignalUp: (state) => {
            const signal = state.ui.selection.signal;
            const index = state.order.findIndex((sig) => sig === signal);
            state.order = [...state.order.slice(0, index - 1), state.order[index], state.order[index - 1], ...state.order.slice(index + 1)];
        },

        moveSelectedSignalDown: (state) => {
            const signal = state.ui.selection.signal;
            const index = state.order.findIndex((sig) => sig === signal);
            state.order = [...state.order.slice(0, index), state.order[index + 1], state.order[index], ...state.order.slice(index + 2)];
        },

        duplicateSelectedColumn: (state) => {
            const col = state.ui.selection.columns[0];
            const fragmentName = state.ui.selection.fragment;
            const fragment = state.instructionFragments[fragmentName];
            const signals = fragment.signals;

            for (const signalName in signals) {
                const signal = signals[signalName];
                signal.values = [...signal.values.slice(0, col), signal.values[col], ...signal.values.slice(col)]
                signal.captions = [...signal.captions.slice(0, col), signal.captions[col], ...signal.captions.slice(col)]
            }
        },

        moveSelectedColumnLeft: (state) => {
            const col = state.ui.selection.columns[0];
            const fragmentName = state.ui.selection.fragment;
            const fragment = state.instructionFragments[fragmentName];
            const signals = fragment.signals;

            if (col === 0) return;

            for (const signalName in signals) {
                const signal = signals[signalName];
                signal.values = [...signal.values.slice(0, col - 1), signal.values[col], signal.values[col - 1], ...signal.values.slice(col + 1)]
                signal.captions = [...signal.captions.slice(0, col - 1), signal.captions[col], signal.captions[col - 1], ...signal.captions.slice(col + 1)]
            }

            state.ui.selection.columns[0] -= 1;
        },

        moveSelectedColumnRight: (state) => {
            const col = state.ui.selection.columns[0];
            const fragmentName = state.ui.selection.fragment;
            const fragment = state.instructionFragments[fragmentName];
            const signals = fragment.signals;

            if (col === signals['STEP_LEN'].values.length - 1) return;

            for (const signalName in signals) {
                const signal = signals[signalName];
                signal.values = [...signal.values.slice(0, col), signal.values[col + 1], signal.values[col], ...signal.values.slice(col + 2)]
                signal.captions = [...signal.captions.slice(0, col), signal.captions[col + 1], signal.captions[col], ...signal.captions.slice(col + 2)]
            }

            state.ui.selection.columns[0] += 1;
        },

        deleteSelectedColumn: (state) => {
            const col = state.ui.selection.columns[0];
            const fragmentName = state.ui.selection.fragment;
            const fragment = state.instructionFragments[fragmentName];
            const signals = fragment.signals;

            const removingLast = col === signals['STEP_LEN'].values.length - 1;

            for (const signalName in signals) {
                const signal = signals[signalName];
                signal.values = [...signal.values.slice(0, col), ...signal.values.slice(col + 1)]
                signal.captions = [...signal.captions.slice(0, col), ...signal.captions.slice(col + 1)]
            }

            if(removingLast) {
                state.ui.selection.columns[0] -= 1;
            }
        },

        setCurrentInstructionDescription: (state, action) => {
            state.instructions[state.ui.op].desciption = action.payload;
        },

        setSelectedSignalDescription: (state, action) => {
            state.settings[state.ui.selection.signal].desciption = action.payload;
        },

        setSelectedEntryCaption: (state, action) => {
            if(state.ui.selection.columns.length !== 1) return;
            if(!state.ui.selection.fragment) return;
            if(!state.ui.selection.signal) return;
        
            const col = state.ui.selection.columns[0];
            const fragmentName = state.ui.selection.fragment;
            const fragment = state.instructionFragments[fragmentName];
            const signalName = state.ui.selection.signal;
            const signal = fragment.signals[signalName];
            signal.captions[col] = action.payload;
        }
    },

    extraReducers: (builder) => {
        builder
          .addCase(loadData.pending, (state) => {
            state.status = 'loading';
          })
          .addCase(loadData.fulfilled, (state, action) => {
            state.instructions = action.payload.instructions;
            state.instructionFragments = loadInstructionFragments(action.payload.fragments);
            state.settings = loadSignals(action.payload.signals);
            state.order = action.payload.order;
            state.status = 'idle';
          });
      },
});

export const {
    setParameterValue,
    toggleSelectedSignalValues,
    setSelectedSignalValues,
    setCurrentInstruction,
    setHideConstant,
    setSelection,
    setFragmentSelection,
    addSelection,
    clearSelection,
    removeSelection,
    setRowSelection,
    setColumnSelection,
    moveSelectedSignalUp,
    moveSelectedSignalDown,
    duplicateSelectedColumn,
    deleteSelectedColumn,
    moveSelectedColumnLeft,
    moveSelectedColumnRight,
    setCurrentInstructionDescription,
    setSelectedSignalDescription,
    setSelectedEntryCaption,
} = timingDiagramSlice.actions;

export const selectOrder = (state) => state.timingDiagram.order;

export const selectCurrentInstruction = (state) => state.timingDiagram.ui.op;
export const selectHideConstant = (state) => state.timingDiagram.ui.hideConstant;
export const selectSelection = (state) => state.timingDiagram.ui.selection;

export const selectSelectedSignalOptions = (state) => getSelectedSignalOptions(state.timingDiagram);
export const selectSelectedSignalValues = (state) => getSelectedSignalValues(state.timingDiagram);

export const selectSelectedColumnMovableDirections = (state) => {
    const col = state.timingDiagram.ui.selection.columns[0];
    const fragmentName = state.timingDiagram.ui.selection.fragment;
    const fragment = state.timingDiagram.instructionFragments[fragmentName];
    const signals = fragment.signals;

    return {
        left: col > 0,
        right: col < signals['STEP_LEN'].values.length - 1,
    };
}

export const selectInstructions = (state) => state.timingDiagram.instructions;
export const selectInstructionFragments = (state) => state.timingDiagram.instructionFragments;
export const selectSettings = (state) => state.timingDiagram.settings;
export const selectInstructionNames = (state) => Object.keys(selectInstructions(state));

export const selectCurrentInstructionDescription = (state) => state.timingDiagram.instructions[state.timingDiagram.ui.op].desciption ?? '';
export const selectSelectedSignalDescription = (state) => state.timingDiagram.settings[state.timingDiagram.ui.selection.signal].desciption ?? '';

export const selectSelectedEntryCaption = (state) => {
    if(state.timingDiagram.ui.selection.columns.length !== 1) return;
    if(!state.timingDiagram.ui.selection.fragment) return;
    if(!state.timingDiagram.ui.selection.signal) return;

    const col = state.timingDiagram.ui.selection.columns[0];
    const fragmentName = state.timingDiagram.ui.selection.fragment;
    const fragment = state.timingDiagram.instructionFragments[fragmentName];
    const signalName = state.timingDiagram.ui.selection.signal;
    const signal = fragment.signals[signalName];
    return signal.captions[col];
};

export const selectDigitalMicrocodeFile = createSelector(
    selectInstructions, selectInstructionFragments, selectSettings,
    (instructions, instructionFragments, signals) => formatDigitalHex(generateMicrocode(instructions, instructionFragments, signals))
);



// export const selectInstruction = (state) => assembleInstruction(selectCurrentInstruction(state), selectInstructions(state), selectInstructionFragments(state));
// export const selectData = (state) => generateInstructionTimeSeries(selectInstruction(state), selectSettings(state));
// export const selectGrid = (state) => generateGrid(selectInstruction(state));

export const selectInstruction = createSelector(
    selectCurrentInstruction, selectInstructions, selectInstructionFragments,
    (op, instructions, fragments) => assembleInstruction(op, instructions, fragments)
);

export const selectData = createSelector(
    selectInstruction, selectSettings,
    (instruction, settings) => generateInstructionTimeSeries(instruction, settings)
)

export const selectGrid = createSelector(
    selectInstruction,
    (instruction) => generateGrid(instruction)
);


export const selectSignalLength = (state) => selectData(state)['STEP_LEN'].reduce((a, b) => a + (b.value || 0), 0);

export const selectParameterTable = createSelector(
    selectCurrentInstruction, selectInstructions, selectInstructionFragments,
    (op, instructions, instructionFragments) => {
        const instruction = instructions[op];
        const result = [];
        instruction.fragments.forEach((fragment) => {
            const paramNames = Object.keys(instructionFragments[fragment.name].parameters);
            paramNames.forEach((paramName) => {
                const param = instructionFragments[fragment.name].parameters[paramName];

                result.push({
                    fragment: fragment.name,
                    signal: param.signal,
                    parameter: paramName,
                    values: param.values,
                    value: fragment.parameters[paramName],
                });
            });
        });
        return result;
    }
)

export default timingDiagramSlice.reducer;