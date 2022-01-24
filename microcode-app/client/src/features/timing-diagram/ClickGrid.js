import { useDispatch, useSelector } from 'react-redux';
import { selectSelection, setSelection, addSelection, removeSelection, setColumnSelection, toggleSelectedSignalValues, setFragmentSelection } from './timingDiagramSlice';



const numberRange = (n1, n2) => {
    const result = [];
    if (n2 > n1) {
        for (let num = n1; num < n2; num++) {
            result.push(num)
        }
    } else {
        for (let num = n1; num > n2; num--) {
            result.push(num)
        }
    }
    return result;
}

const GridRect = (props) => {
    const { signal, fragment, col, caption, warning, dim, generated } = props;

    const dispatch = useDispatch();
    const selection = useSelector(selectSelection);

    const selected = (selection.fragment === fragment) && (selection.signal === signal) && selection.columns.includes(col);
    const sameRow = (selection.fragment === fragment) && (selection.signal === signal);

    const onClick = (ev) => {
        if (signal === '[fragment]') {
            dispatch(setFragmentSelection(fragment));
        } else if (generated) {
            dispatch(setColumnSelection({ fragment, col }));
        } else {
            if (ev.ctrlKey) {
                if (!selected) {
                    dispatch(addSelection({ fragment, signal, columns: [col] }));
                } else {
                    dispatch(removeSelection({ fragment, signal, columns: [col] }));
                }
            } else if (ev.shiftKey && sameRow) {
                const prevCol = selection.columns[selection.columns.length - 1];
                const columns = numberRange(col, prevCol).reverse();
                dispatch(addSelection({ fragment, signal, columns }));
            } else {
                dispatch(setSelection({ fragment, signal, col }));
            }
        }
    }

    return (
        <g>
            {warning && <text
                x={props.x + 0.5 * dim.tickWidth}
                y={props.y + 0.5 * props.height}
                alignmentBaseline="middle"
                dominantBaseline="middle"
                textAnchor="middle"
                fontFamily="sans-serif"
                fontSize="5pt"
                style={{ 'pointerEvents': 'none', 'userSelect': 'none' }}>!</text>}
            {(caption && !warning) && <text
                x={props.x + 0.5 * dim.tickWidth}
                y={props.y + 0.5 * props.height}
                alignmentBaseline="middle"
                dominantBaseline="middle"
                textAnchor="middle"
                fontFamily="sans-serif"
                fontSize="5pt"
                style={{ 'pointerEvents': 'none', 'userSelect': 'none' }}>i</text>}
            <rect x={props.x} y={props.y} width={props.width} height={props.height}
                fill="transparent"
                onClick={(ev) => { onClick(ev); ev.stopPropagation(); ev.nativeEvent.stopImmediatePropagation(); ev.preventDefault(); }}
                onDoubleClick={(ev) => { dispatch(toggleSelectedSignalValues()); ev.stopPropagation(); ev.nativeEvent.stopImmediatePropagation(); ev.preventDefault(); }}
            >
                <title>
                    {warning || caption}
                </title>
            </rect>
        </g>
    );
}

export const ClickGrid = (props) => {
    const { dim } = props;

    const lastEntry = props.grid[props.grid.length - 1];
    return (<g>
        {
            props.grid.map((entry, index) => (
                <line
                    key={entry.time.toString()}
                    x1={entry.time * dim.tickWidth}
                    x2={entry.time * dim.tickWidth}
                    y1={0.5 * (dim.signalDistance - dim.signalHeight)}
                    y2={props.signals.length * dim.signalDistance - 0.5 * (dim.signalDistance - dim.signalHeight)}
                    strokeWidth={1}
                    stroke="gray"
                    strokeDasharray={((index === 0) || (entry.fragment !== props.grid[index - 1].fragment)) ? "1,1" : "1,3"}
                />
            ))
        }

        <line
            key={(lastEntry.time + lastEntry.duration).toString()}
            x1={(lastEntry.time + lastEntry.duration) * dim.tickWidth}
            x2={(lastEntry.time + lastEntry.duration) * dim.tickWidth}
            y1={0.5 * (dim.signalDistance - dim.signalHeight)}
            y2={props.signals.length * dim.signalDistance - 0.5 * (dim.signalDistance - dim.signalHeight)}
            strokeWidth={1}
            stroke="gray"
            strokeDasharray="1,1"
        />

        {
            props.signals.map((signal, rowIndex) => props.grid.map((entry, colIndex) =>
                <GridRect key={signal + colIndex.toString()} col={entry.col} fragment={entry.fragment} signal={signal} op={props.op}
                    caption={props.instruction[signal]?.captions?.[colIndex] || ''}
                    warning={props.instruction[signal]?.warnings?.[colIndex]}
                    x={dim.tickWidth * entry.time}
                    y={rowIndex * dim.signalDistance}
                    width={dim.tickWidth * entry.duration}
                    height={dim.signalDistance}
                    dim={dim}
                    generated={props.signalSettings[signal].generated}
                />
            ))
        }
    </g>);
}