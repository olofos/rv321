import { configureStore } from '@reduxjs/toolkit';
import counterReducer from '../features/counter/counterSlice';
import timingDiagramReducer from '../features/timing-diagram/timingDiagramSlice'

export const store = configureStore({
  reducer: {
    counter: counterReducer,
    timingDiagram: timingDiagramReducer,
  },
});
