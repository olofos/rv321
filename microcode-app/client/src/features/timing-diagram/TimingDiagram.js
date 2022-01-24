import { useSelector, useDispatch } from 'react-redux';
import React, { useState } from 'react';

import { selectData, selectSettings, selectSignalLength, selectOrder, selectInstruction, selectGrid, selectHideConstant, selectSelection, setRowSelection } from './timingDiagramSlice';

import { Signals } from './signal';
import { ClickGrid } from './ClickGrid';
import { BackgroundGrid } from './BackgroundGrid';

export const TimingDiagram = (props) => {
    const dispatch = useDispatch();

    const instruction = useSelector(selectInstruction);
    const signals = useSelector(selectData);
    const signalSettings = useSelector(selectSettings);
    const signalLength = useSelector(selectSignalLength);
    const signalNames = useSelector(selectOrder);
    const grid = useSelector(selectGrid);
    const hideConstant = useSelector(selectHideConstant);
    const selection = useSelector(selectSelection);

    const [scroll, setScroll] = useState(0);

    const filteredSignalNames = signalNames.filter(key => !hideConstant || (signals[key].length > 1));

    const margin = 10;

    const signalWidth = (signalLength + 1) * props.tickWidth;

    const { tickWidth, signalHeight, signalDistance, slope, busSlope } = props;
    const dim = { tickWidth, signalHeight, signalDistance, slope, busSlope };
    // const dim = { tickWidth: props.tickWidth, signalHeight: props.signalHeight, signalDistance: props.signalDistance, slope: props.slope, busSlope: props.busSlope };
    const totalHeight = filteredSignalNames.length * props.signalDistance

    return (
        <div style={{ overflow: 'hidden', padding: '20px' }}>
            <div style={{ maxWidth: '100%', maxHeight: '100%', overflow: 'auto', display: 'inline-block' }} onScroll={(ev) => setScroll(ev.target.scrollLeft)}>
                <svg width={props.nameWidth + margin + signalWidth} height={totalHeight} style={{ paddingRight: '4px' }}>
                    <defs>
                        <clipPath id="signal-region">
                            <rect x={props.nameWidth + margin} y="0" width={signalWidth} height={totalHeight} />
                        </clipPath>
                    </defs>
                    {false && <g transform={`translate(0,0)`}>
                        {
                            filteredSignalNames.map((key, index) => {
                                const rowSelected = ((selection.fragment === null) && (selection.signal === key) && (selection.columns.length === 0));
                                return <rect
                                    key={key}
                                    x={0}
                                    y={dim.signalDistance * index}
                                    width={props.nameWidth + signalWidth + margin}
                                    height={dim.signalDistance}
                                    fill={rowSelected ? '#aae' : (((index % 2) === 0) ? '#ddd' : '#eee')}
                                />
                            })
                        }
                    </g>}
                    <g
                    // clipPath="url(#signal-region)"
                    >
                        <g transform={`translate(${props.nameWidth + 0.5 * dim.tickWidth + margin},0)`}>

                            {
                                // signals['STEP_LEN'].slice(1).map(entry => <line key={entry.time.toString()} x1={dim.tickWidth * entry.time} x2={dim.tickWidth * entry.time} y1={0.5 * (dim.signalDistance - dim.signalHeight)} y2={(filteredSignalNames.length) * dim.signalDistance - 0.5 * (dim.signalDistance - dim.signalHeight)} strokeWidth={1} stroke="gray" strokeDasharray="1,3" />)
                            }

                            {/* {
                                filteredSignalNames.map((key, index) => <Signal type={signalSettings[key].type} key={key} data={signals[key]} name={key} index={index} clk={signalSettings[key].clk} dim={dim} />)
                            } */}
                            <BackgroundGrid dim={dim} signals={filteredSignalNames} grid={grid} instruction={instruction} updateScroll={() => { }} signalSettings={signalSettings} />

                            <Signals {...{ filteredSignalNames, signalSettings, signals, dim, instruction }} />

                            {/* <rect x="-180" y="0" width="1000" height="1000" fillOpacity="0.2" /> */}
                            <ClickGrid dim={dim} signals={filteredSignalNames} grid={grid} instruction={instruction} updateScroll={() => { }} signalSettings={signalSettings} />
                        </g>
                    </g>
                    <g transform={`translate(${scroll},0)`}>
                        {
                            filteredSignalNames.map((key, index) => {
                                const rowSelected = ((selection.fragment === null) && (selection.signal === key) && (selection.columns.length === 0));
                                return (
                                    <g key={key}>
                                        <rect
                                            x={0}
                                            y={dim.signalDistance * index}
                                            height={dim.signalDistance}
                                            width={props.nameWidth + margin}
                                            fill={rowSelected ? '#aae' : (((index % 2) === 0) ? '#ddd' : '#eee')}
                                        />
                                        {(key[0] !== '[') && <g>
                                            <text
                                                x={props.nameWidth}
                                                y={dim.signalDistance * (index + 0.5)}
                                                alignmentBaseline="middle"
                                                dominantBaseline="middle"
                                                textAnchor="end"
                                                fontFamily="monospace"
                                                fontSize="10pt"
                                                fontStyle={signalSettings[key].generated ? 'italic' : 'normal'}
                                                fontWeight={(selection.signal === key) ? 'bold' : 'normal'}
                                                textDecoration={key[0] === '~' ? 'overline' : ''}
                                                style={{ 'pointerEvents': 'none', 'userSelect': 'none' }}
                                                stroke="transparent"
                                                fill="black"
                                            >
                                                {key[0] === '~' ? key.slice(1) : key}
                                            </text>
                                            <rect
                                                x={0}
                                                y={dim.signalDistance * index}
                                                width={props.nameWidth}
                                                height={dim.signalDistance}
                                                fillOpacity="0.2"
                                                fill="transparent"
                                                onClick={(ev) => { ev.stopPropagation(); ev.nativeEvent.stopImmediatePropagation(); dispatch(setRowSelection(key)); }}
                                                onDoubleClick={(ev) => { ev.stopPropagation(); ev.nativeEvent.stopImmediatePropagation(); }}
                                            />
                                        </g>}
                                    </g>);

                            })
                        }
                    </g>
                </svg >
            </div>
        </div>
    );
}
