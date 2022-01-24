import { configureStore } from '@reduxjs/toolkit';
import timingDiagramReducer from '../features/timing-diagram/timingDiagramSlice'

export const store = configureStore({
  reducer: {
    timingDiagram: timingDiagramReducer,
  },
});
