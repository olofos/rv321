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

  if(status !== 'idle') return (
    <div>Loading</div>
  );

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
}

export default App;
