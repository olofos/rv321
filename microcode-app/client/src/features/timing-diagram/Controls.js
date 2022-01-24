import { useSelector, } from 'react-redux';

import { InstructionControl } from './InstructionControl';
import { EntryControl } from './EntryControl';
import { RowControl } from './RowControl';
import { ColumnControl } from './ColumnControl';
import { FragmentControl } from './FragmentControl';
import {
    selectSelection,
} from './timingDiagramSlice';

export const Controls = () => {
    const selection = useSelector(selectSelection);

    const entrySelected = selection.signal && selection.fragment && (selection.columns.length > 0);
    const rowSelected = selection.signal && !selection.fragment && (selection.columns.length === 0);
    const columnSelected = !selection.signal && selection.fragment && (selection.columns.length === 1);
    const fragmentSelected = !selection.signal && selection.fragment && (selection.columns.length === 0);
    const nothingSelected = !entrySelected && !rowSelected && !columnSelected && !fragmentSelected;

    return (
        <div>
            {nothingSelected && <InstructionControl />}
            {entrySelected && <EntryControl />}
            {rowSelected && <RowControl />}
            {columnSelected && <ColumnControl />}
            {fragmentSelected && <FragmentControl />}
        </div>
    );
}
