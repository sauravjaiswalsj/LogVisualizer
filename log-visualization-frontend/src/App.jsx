import './App.css'
import React, { useState, useEffect } from 'react';
import { LineChart, Line, XAxis, YAxis, Tooltip, ResponsiveContainer } from 'recharts';
import { 
  CalendarIcon, 
  FilterIcon, 
  DownloadIcon, 
  RefreshCwIcon 
} from 'lucide-react';

const App = () => {
  // State management
  const [logs, setLogs] = useState([]);
  const [filteredLogs, setFilteredLogs] = useState([]);
  const [searchTerm, setSearchTerm] = useState('');
  const [logLevel, setLogLevel] = useState('All');
  const [timeRange, setTimeRange] = useState('24h');
  const [isLoading, setIsLoading] = useState(false);

  // Fetch logs from backend
  const fetchLogs = async () => {
    setIsLoading(true);
    try {
      const response = await fetch('http://localhost:8080/api/logs', {
        method: 'GET',
        headers: {
          'Accept': 'application/json'
        }
      });

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }

      const data = await response.json();
      setLogs(data);
      setFilteredLogs(data);
    } catch (error) {
      console.error("Could not fetch logs:", error);
    } finally {
      setIsLoading(false);
    }
  };

  // Effect to fetch logs on component mount
  useEffect(() => {
    fetchLogs();
    // Setup real-time polling
    const interval = setInterval(fetchLogs, 60000); // Refresh every minute
    return () => clearInterval(interval);
  }, []);

  // Filtering logic
  useEffect(() => {
    let result = logs;

    // Search filter
    if (searchTerm) {
      result = result.filter(log => 
        log.message.toLowerCase().includes(searchTerm.toLowerCase())
      );
    }

    // Log level filter
    if (logLevel !== 'All') {
      result = result.filter(log => log.level === logLevel);
    }

    // Time range filter
    const now = new Date();
    const filterDate = new Date();
    switch(timeRange) {
      case '1h': 
        filterDate.setHours(now.getHours() - 1); 
        break;
      case '6h': 
        filterDate.setHours(now.getHours() - 6); 
        break;
      case '24h': 
        filterDate.setDate(now.getDate() - 1); 
        break;
      case '7d': 
        filterDate.setDate(now.getDate() - 7); 
        break;
    }

    result = result.filter(log => new Date(log.timestamp) >= filterDate);

    setFilteredLogs(result);
  }, [searchTerm, logLevel, timeRange, logs]);

  // Log volume data for timeline
  const logVolumeData = React.useMemo(() => {
    const volumeMap = {};
    filteredLogs.forEach(log => {
      const date = new Date(log.timestamp).toLocaleDateString();
      volumeMap[date] = (volumeMap[date] || 0) + 1;
    });

    return Object.entries(volumeMap).map(([date, count]) => ({
      date, 
      logs: count
    }));
  }, [filteredLogs]);

  // Log level statistics
  const logLevelStats = React.useMemo(() => {
    const stats = {
      Error: 0,
      Warning: 0,
      Info: 0
    };
    filteredLogs.forEach(log => {
      stats[log.level] = (stats[log.level] || 0) + 1;
    });
    return stats;
  }, [filteredLogs]);

  // Export functionality
  const exportLogs = () => {
    const csvContent = filteredLogs.map(log => 
      `${log.timestamp},${log.level},${log.message}`
    ).join('\n');
    
    const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
    const link = document.createElement('a');
    const url = URL.createObjectURL(blob);
    link.setAttribute('href', url);
    link.setAttribute('download', 'logs_export.csv');
    link.style.visibility = 'hidden';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  return (
    <div className="container mx-auto p-4 space-y-4">
      {/* Header Section */}
      <div className="flex justify-between items-center">
        <h1 className="text-2xl font-bold">Log Visualization Dashboard</h1>
        <div className="space-x-2">
          <button 
            onClick={fetchLogs} 
            className="px-4 py-2 border rounded hover:bg-gray-100 flex items-center"
          >
            <RefreshCwIcon className="mr-2 h-4 w-4" /> Refresh
          </button>
          <button 
            onClick={exportLogs}
            className="px-4 py-2 bg-blue-500 text-white rounded hover:bg-blue-600 flex items-center"
          >
            <DownloadIcon className="mr-2 h-4 w-4" /> Export
          </button>
        </div>
      </div>

      {/* Search and Filter Bar */}
      <div className="bg-white shadow rounded-lg p-4">
        <div className="flex space-x-4">
          <input 
            placeholder="Search logs..." 
            value={searchTerm}
            onChange={(e) => setSearchTerm(e.target.value)}
            className="flex-grow px-3 py-2 border rounded"
          />
          <select 
            value={logLevel} 
            onChange={(e) => setLogLevel(e.target.value)}
            className="px-3 py-2 border rounded"
          >
            <option value="All">All Levels</option>
            <option value="ERROR">Error</option>
            <option value="WARNING">Warning</option>
            <option value="INFO">Info</option>
          </select>
          <select 
            value={timeRange} 
            onChange={(e) => setTimeRange(e.target.value)}
            className="px-3 py-2 border rounded"
          >
            <option value="1h">Last 1 Hour</option>
            <option value="6h">Last 6 Hours</option>
            <option value="24h">Last 24 Hours</option>
            <option value="7d">Last 7 Days</option>
          </select>
        </div>
      </div>

      {/* Log Statistics and Timeline */}
      <div className="grid grid-cols-3 gap-4">
        {/* Log Statistics Card */}
        <div className="bg-white shadow rounded-lg p-4">
          <h2 className="text-lg font-semibold mb-3">Log Statistics</h2>
          <div className="space-y-2">
            <div>Error Logs: {logLevelStats.Error}</div>
            <div>Warning Logs: {logLevelStats.Warning}</div>
            <div>Info Logs: {logLevelStats.Info}</div>
          </div>
        </div>

        {/* Log Activity Timeline */}
        <div className="col-span-2 bg-white shadow rounded-lg p-4">
          <h2 className="text-lg font-semibold mb-3">Log Activity Timeline</h2>
          <ResponsiveContainer width="100%" height={200}>
            <LineChart data={logVolumeData}>
              <XAxis dataKey="date" />
              <YAxis />
              <Tooltip />
              <Line type="monotone" dataKey="logs" stroke="#8884d8" />
            </LineChart>
          </ResponsiveContainer>
        </div>
      </div>

      {/* Recent Logs Table */}
      <div className="bg-white shadow rounded-lg p-4">
        <h2 className="text-lg font-semibold mb-3">Recent Logs</h2>
        <table className="w-full">
          <thead>
            <tr className="border-b">
              <th className="py-2 text-left">Timestamp</th>
              <th className="py-2 text-left">Level</th>
              <th className="py-2 text-left">Message</th>
            </tr>
          </thead>
          <tbody>
            {filteredLogs.slice(0, 10).map((log, index) => (
              <tr key={index} className="border-b">
                <td className="py-2">{new Date(log.timestamp).toLocaleString()}</td>
                <td className="py-2">{log.level}</td>
                <td className="py-2">{log.message}</td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
};

export default App
