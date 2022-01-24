import { useSelector, useDispatch } from 'react-redux';

import {
    selectOrder,
    selectSelection,
    moveSelectedSignalUp,
    moveSelectedSignalDown,
    selectSelectedSignalDescription,
    setSelectedSignalDescription,
} from './timingDiagramSlice';


export const RowControl = () => {
    const dispatch = useDispatch();
    const selection = useSelector(selectSelection);
    const signalOrder = useSelector(selectOrder);
    const desciption = useSelector(selectSelectedSignalDescription);

    return (
        <form onSubmit={(ev) => { ev.preventDefault(); }}>
            <button onClick={(ev) => { dispatch(moveSelectedSignalUp()); ev.preventDefault(); }} disabled={selection.signal === signalOrder[1]}>Up</button>
            <button onClick={(ev) => { ev.preventDefault(); dispatch(moveSelectedSignalDown()); }} disabled={selection.signal === signalOrder[signalOrder.length - 2]}>Down</button>
            <label htmlFor="description">Description:</label>
            <textarea name="description" value={desciption} onChange={(ev) => dispatch(setSelectedSignalDescription(ev.target.value))} />
        </form>
    );
}
