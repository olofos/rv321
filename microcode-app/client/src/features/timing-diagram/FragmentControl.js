import { useSelector, useDispatch } from 'react-redux';

import {
    selectCurrentInstruction,
    selectParameterTable,

    setParameterValue,
    selectSelection
} from './timingDiagramSlice';


export const FragmentControl = () => {
    const dispatch = useDispatch();
    const selection = useSelector(selectSelection);
    const instruction = useSelector(selectCurrentInstruction);
    const parameterTable = useSelector(selectParameterTable).filter(entry => entry.fragment === selection.fragment);

    return (<form>
        <div>
            <table>
                <thead>
                    <tr>
                        <th>Fragment</th>
                        <th>Parameter</th>
                        <th>Signal</th>
                        <th>Value</th>
                    </tr>
                </thead>
                <tbody>
                    {parameterTable.map((entry) => (
                        <tr key={entry.fragment + entry.parameter}>
                            <td>{entry.fragment}</td>
                            <td>{entry.parameter}</td>
                            <td>{entry.signal}</td>
                            <td>
                                <select value={entry.value} onChange={(ev) => dispatch(setParameterValue({ instruction, fragment: entry.fragment, parameter: entry.parameter, value: ev.target.value }))}>
                                    {
                                        entry.values.map((value) => (
                                            <option key={value} value={value}>{value}</option>
                                        ))
                                    }
                                </select>
                            </td>
                        </tr>
                    ))}
                </tbody>
            </table>
        </div>
    </form>);
}