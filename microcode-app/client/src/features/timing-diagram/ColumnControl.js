import { useSelector, useDispatch } from 'react-redux';

import {
    selectSelectedColumnMovableDirections,
    moveSelectedColumnLeft,
    moveSelectedColumnRight,
    duplicateSelectedColumn,
    deleteSelectedColumn,
} from './timingDiagramSlice';


export const ColumnControl = () => {
    const dispatch = useDispatch();

    const directions = useSelector(selectSelectedColumnMovableDirections);

    return (
        <form onSubmit={(ev) => { ev.preventDefault(); }}>
            <button disabled={!directions.left} onClick={(ev) => { ev.preventDefault(); dispatch(moveSelectedColumnLeft()); }}>Left</button>
            <button disabled={!directions.right} onClick={(ev) => { ev.preventDefault(); dispatch(moveSelectedColumnRight()); }}>Right</button>

            <button onClick={(ev) => { ev.preventDefault(); dispatch(duplicateSelectedColumn()); }}>Duplicate</button>
            <button onClick={(ev) => { ev.preventDefault(); dispatch(deleteSelectedColumn()); }}>Delete</button>
        </form>
    );
}
