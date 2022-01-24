import { useSelector, useDispatch } from 'react-redux';

import {
    selectSelection,
    selectSelectedSignalOptions,
    selectSelectedSignalValues,
    setSelectedSignalValues,
    selectSelectedEntryCaption,
    setSelectedEntryCaption,
} from './timingDiagramSlice';


export const EntryControl = () => {
    const dispatch = useDispatch();
    const signalOptions = useSelector(selectSelectedSignalOptions);
    const signalValues = useSelector(selectSelectedSignalValues);
    const selection = useSelector(selectSelection);
    const caption = useSelector(selectSelectedEntryCaption);

    return (
        <form>
            <label htmlFor="value">Value:</label>
            <select name="value" value={(signalValues.length === 1) ? signalValues[0] : "-- select an option --"} onChange={(ev) => dispatch(setSelectedSignalValues(ev.target.value))}>
                {(signalValues.length === 1) || <option disabled>-- select an option --</option>}
                {signalOptions.map((value) => {
                    if (value.length === 0) { return <option key={value} value={value} style={{ fontStyle: "italic" }}>Do not care</option>; }
                    if (value[0] === '{') { return <option key={value} value={value} style={{ fontStyle: "italic" }}>{'Parameter: ' + value.slice(1, -1)}</option>; }
                    return <option key={value} value={value}>{value}</option>;
                })}
            </select>
            <label htmlFor="caption">Caption:</label>
            <textarea name="caption" value={caption} disabled={(selection.columns.length > 1)} onChange={(ev) => dispatch(setSelectedEntryCaption(ev.target.value))} />
        </form>
    );
}