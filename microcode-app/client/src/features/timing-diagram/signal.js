import React from 'react';

function coords(dim, time, value) {
    return {
        x: time,
        y: (0.5 - value),
    }
}

function get_bin_path(data, dim, dx) {
    let points = [coords(dim, data[0].time, data[0].value)];
    for (let i = 1; i < data.length; i++) {
        points.push(coords(dim, data[i].time, data[i - 1].value));

        if (data[i - 1].value !== data[i].value) {
            points.push(coords(dim, data[i].time + dx, data[i].value));
        }
    }
    const last = data[data.length - 1];
    points.push(coords(dim, last.time + last.duration, last.value));
    return 'M ' + points.map((p) => `${p.x} ${p.y}`).join(' L ');
}

function get_alt_bin_path(data, dim, dx) {
    const segments = [];
    for(let index = 1; index < data.length - 1; index++) {
        if(data[index].parameter) {
            segments.push({...data[index], pre: data[index-1].value, post: data[index+1].value});
        }
    }
    const paths = [];
    segments.forEach((segment) => {
        const value = 1 - segment.value;
        let points = [
            coords(dim, segment.time, segment.pre),
            coords(dim, segment.time + dx, value),
            coords(dim, segment.time + segment.duration, value),
            coords(dim, segment.time + segment.duration + dx, segment.post),
        ];
        paths.push('M ' + points.map((p) => `${p.x} ${p.y}`).join(' L '));
    })
    return paths.join(' ');
}

function get_box_path(start, end, dim, dx) {

    let points = [
        coords(dim, start, 0.5), coords(dim, (start + dx / 2), 0),
        coords(dim, end - dx / 2, 0), coords(dim, end, 0.5),
        coords(dim, end - dx / 2, 1), coords(dim, (start + dx / 2), 1),
        coords(dim, start, 0.5),
    ]

    return 'M ' + points.map((p) => `${p.x} ${p.y}`).join(' L ');
}

function signalMiddle(index, dim) {
    return dim.signalDistance * (index + 0.5);
}
function signalTransform(index, dim) {
    return `translate(0,${signalMiddle(index, dim)}) scale(${dim.tickWidth},${dim.signalHeight})`;
}

export const BinSignal = (props) => {
    const { data, dim, warnings } = props;

    const path = get_bin_path(data, dim, props.clk ? 0 : dim.slope);
    const altPath = get_alt_bin_path(data, dim, props.clk ? 0 : dim.slope);

    return (
        <g>
            <g transform={signalTransform(props.index,dim)}>
                { altPath && <path d={altPath} stroke="black" fill="transparent" strokeWidth="0.7" strokeDasharray="3,3" vectorEffect="non-scaling-stroke" /> }
                <path d={path} stroke={warnings ? "red" : "black"} fill="transparent" strokeWidth="1" vectorEffect="non-scaling-stroke" />
            </g >
            {data.map((entry, index) => {
                return entry.parameter && (<g key={index.toString()}>
                    <text
                        x={`${dim.tickWidth * (entry.time + entry.duration / 2)}`}
                        y={signalMiddle(props.index, dim) + 0.5}
                        alignmentBaseline="middle"
                        dominantBaseline="middle"
                        textAnchor="middle"
                        fontSize="6pt"
                        style={{ 'pointerEvents': 'none', 'userSelect': 'none' }}>
                        {'(' + entry.parameter.slice(1, -1) + ')'}
                    </text>
                </g>)
            })}
        </g>
    );
}


export const BusSignal = (props) => {
    const { data, dim, warnings } = props;
    if (props.clk) {
        dim.slope = 0;
    }

    return (<g>
        <g transform={signalTransform(props.index, dim)}>
            {data.map((entry, index) => {
                const path = get_box_path(entry.time, entry.time + entry.duration, dim, props.clk ? 0 : dim.busSlope);
                if (entry.value !== '') {
                    return (
                        <g key={index.toString()}>
                            <path d={path} stroke={warnings ? "red" : "black"} fill="white" fillOpacity={0.2} strokeWidth="1" vectorEffect="non-scaling-stroke" />
                        </g>);
                } else {
                    return (
                        <g key={index.toString()}>
                            <path d={path} stroke="black" fill="gray" fillOpacity="0.2" strokeWidth="1" vectorEffect="non-scaling-stroke" />
                        </g>);
                }
            })}
            {/* <text x="-1" y="0" alignmentBaseline="middle" textAnchor="end" fontSize="14pt" >{props.name}</text> */}
        </g>
        {data.map((entry, index) => {
            return (<g key={index.toString()}>
                <text
                    x={`${dim.tickWidth * (entry.time + entry.duration / 2)}`}
                    y={signalMiddle(props.index, dim) + 1}
                    alignmentBaseline="middle"
                    dominantBaseline="middle"
                    textAnchor="middle"
                    fontSize="6pt"
                    style={{ 'pointerEvents': 'none', 'userSelect': 'none' }}>
                    {entry.value + (entry.parameter ? (' (' + entry.parameter.slice(1, -1) + ')') : '')}
                </text>
            </g>)
        })}
    </g>)
}

export const Signal = (props) => {
    // console.log([props.name,props.generated])
    const Component = (props.type === 'binary') ? BinSignal : BusSignal;
    return (<g>
        <Component {...props} />
    </g>);
}

export const Signals = React.memo((props) => {
    // console.log(props)
    const { filteredSignalNames, signalSettings, signals, dim, instruction } = props;
    return filteredSignalNames.map((key, index) => <Signal type={signalSettings[key].type} key={key} data={signals[key]} name={key} index={index} clk={signalSettings[key].clk} dim={dim} warnings={instruction[key]?.warnings} />)
}, (newProps, oldProps) => JSON.stringify(newProps) === JSON.stringify(oldProps))
