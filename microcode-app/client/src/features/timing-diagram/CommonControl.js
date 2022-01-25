import { useSelector, useDispatch } from 'react-redux';

import {
    selectInstructionNames,
    selectCurrentInstruction,
    selectHideConstant,
    selectDigitalMicrocodeFile,

    setCurrentInstruction,
    setHideConstant,
    loadData,
    saveData,
} from './timingDiagramSlice';

function doExport(microcode) {
    const data = new Blob([microcode], { type: 'text/plain' });
    const url = window.URL.createObjectURL(data);
    const tempLink = document.createElement('a');
    tempLink.href = url;
    tempLink.setAttribute('download', 'microcode.hex');
    tempLink.click();
}

function doExit() {
    fetch('/api/exit', {
        method: 'POST',
        body: 'exit',
    }).then(() =>  window.close());
}

export const CommonControl = () => {
    const dispatch = useDispatch();

    const instruction = useSelector(selectCurrentInstruction);
    const instructionNames = useSelector(selectInstructionNames);
    const hideConstant = useSelector(selectHideConstant);
    const microcode = useSelector(selectDigitalMicrocodeFile);

    return (<form onSubmit={(ev) => { ev.preventDefault(); }}>
        <div>
            <label htmlFor="instruction">Instruction:</label>
            <select name="instruction" value={instruction} onChange={(ev) => dispatch(setCurrentInstruction(ev.target.value))}>
                {instructionNames.map(op => <option key={op} value={op}>{op}</option>)}
            </select>
        </div>
        <div>
            <button onClick={(ev) => { ev.preventDefault(); dispatch(saveData()); }}>Save</button>
            <button onClick={(ev) => { ev.preventDefault(); doExport(microcode); }}>Export</button>
            <button onClick={(ev) => { ev.preventDefault(); dispatch(loadData()); }}>Revert</button>
            <button onClick={(ev) => { ev.preventDefault(); doExit(); }}>Exit</button>
            <label htmlFor="hideConstant">Hide constant signals:</label>
            <input type="checkbox" name="hideConstant" checked={hideConstant} onChange={(ev) => dispatch(setHideConstant(!hideConstant))} />
        </div>
    </form>);
}