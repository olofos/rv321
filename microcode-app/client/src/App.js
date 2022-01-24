import { useEffect } from 'react';
import { useDispatch, useSelector } from 'react-redux';

import { TimingDiagram } from './features/timing-diagram/TimingDiagram';
import { CommonControl } from './features/timing-diagram/CommonControl';
import { Controls } from './features/timing-diagram/Controls';


import {
  clearSelection,
  toggleSelectedSignalValues,
  loadData,
} from './features/timing-diagram/timingDiagramSlice';

import './App.css';

function App() {
  const dispatch = useDispatch();
  const status = useSelector((state) => state.timingDiagram.status);

  useEffect(() => dispatch(loadData()), [dispatch]);

  if(status !== 'idle') return null;

  return (
    <div className="App" id="App" style={{
      width: '100%',
      height: '100%',
      display: 'grid',
      gridTemplateRows: '100px 1fr 200px',
      gridTemplateColumns: '1fr',
    }}
      tabIndex={-1}
      onKeyPress={(ev) => { if ((ev.key === ' ') && (!['SELECT', 'TEXTAREA', 'INPUT', 'BUTTON'].includes(ev.target.tagName))) { dispatch(toggleSelectedSignalValues()); } }}
      onClick={(ev) => { if (!['SELECT', 'TEXTAREA', 'INPUT', 'BUTTON'].includes(ev.target.tagName)) { dispatch(clearSelection()); } }}
    >
      <CommonControl />
      <TimingDiagram nameWidth={115} tickWidth={20} signalHeight={12} signalDistance={24} slope={0.1} busSlope={0.2} />
      <Controls />
    </div>
  );

  // const [scroll, setScroll] = useState(0);

  // return (
  //   <div className="App" id="App" style={{
  //     width: '100%',
  //     height: '100%',
  //     display: 'grid',
  //     gridTemplateRows: '100px 1fr 200px',
  //     gridTemplateColumns: '1fr',
  //   }}
  //     tabIndex={-1}
  //   // onKeyPress={(ev) => { if (ev.key === ' ') { dispatch(toggleSelectedSignalValues()); } }}
  //   // onClick={(ev) => { if (!['SELECT', 'TEXTAREA', 'INPUT', 'BUTTON'].includes(ev.target.tagName)) { dispatch(clearSelection()); } }}
  //   >
  //     <div style={{ width: "400px", height: "400px", gridRow: 2, overflow: "scroll" }} onScroll={(ev) => setScroll(ev.target.scrollLeft)}>
  //       <div style={{ position: "relative" }}>
  //         <svg width={800} height={800}>
  //           {[...Array(40).keys()].map(x => <line key={x} x1={20 * x} x2={20 * x} y1={0} y2={800} stroke="red" />)}
  //           {[...Array(40).keys()].map(y => <line key={y} x1={0} x2={800} y1={20 * y} y2={20 * y} stroke="blue" />)}
  //           <g transform={`translate(${scroll},0)`}>
  //             {[...Array(40).keys()].map(y => <rect key={y} x="0" y={20 * y} width="40" height="20" fill={(y % 2 == 0) ? "green" : "blue"} fillOpacity="0.2" />)}
  //             </g>
  //         </svg>
  //         {/* <div style={{ position: "absolute", top: "0", left: scroll }}>
  //           <svg width={40} height={800}>
  //           </svg>
  //         </div> */}
  //       </div>
  //     </div>
  //   </div>
  // );
}

export default App;
