import { useSelector } from 'react-redux';
import { selectSelection,} from './timingDiagramSlice';

const BackgroundRect = (props) => {
    const { signal, fragment, col, rowIndex } = props;

    const selection = useSelector(selectSelection);

    
    const entrySelected = (selection.fragment === fragment) && (selection.signal === signal) && selection.columns.includes(col);
    const columnSelected = (selection.fragment === fragment) && (selection.signal === null) && selection.columns.includes(col) && (selection.columns.length === 1);
    const rowSelected = (selection.fragment === null) && (selection.signal === signal) && (selection.columns.length === 0);
    const fragmentSelected = (signal === '[fragment]') && (selection.fragment === fragment) && (selection.signal === null) && (selection.columns.length === 0);
    
    const selected = entrySelected || columnSelected || rowSelected || fragmentSelected;

    return (
        <rect x={props.x} y={props.y} width={props.width} height={props.height}
            // fillOpacity="0.2" fill={((row + col) % 2 == 0) ? "red" : "blue"}
            fill={selected ? "#aae" : (((rowIndex % 2) === 0) ? '#ddd' : '#eee')}
        />
    );
}

export const BackgroundGrid = (props) => {
    const { dim } = props;

    const selection = useSelector(selectSelection);

    const last = props.grid[props.grid.length - 1];

    return (<g>
        {
            props.signals.map((signal, rowIndex) => {
                const rowSelected = (selection.fragment === null) && (selection.signal === signal) && (selection.columns.length === 0);
                return <g key={rowIndex.toString()}>
                    <rect
                        
                        x={-dim.tickWidth}
                        y={rowIndex * dim.signalDistance}
                        width={dim.tickWidth}
                        height={dim.signalDistance}
                        fill={(rowSelected) ? "#aae" : (((rowIndex % 2) === 0) ? '#ddd' : '#eee')}
                    />
                    <rect
                        x={dim.tickWidth * (last.time + last.duration)}
                        y={rowIndex * dim.signalDistance}
                        width={dim.tickWidth}
                        height={dim.signalDistance}
                        fill={(rowSelected) ? "#aae" : (((rowIndex % 2) === 0) ? '#ddd' : '#eee')}
                    />
                </g>
            })
        }
        {
            props.signals.map((signal, rowIndex) => props.grid.map((entry, colIndex) =>
                <BackgroundRect key={signal + colIndex.toString()} col={entry.col} fragment={entry.fragment} signal={signal} op={props.op}
                    x={dim.tickWidth * entry.time}
                    y={rowIndex * dim.signalDistance}
                    width={dim.tickWidth * entry.duration}
                    height={dim.signalDistance}
                    dim={dim}
                    rowIndex={rowIndex}
                />
            ))
        }
    </g>);
}