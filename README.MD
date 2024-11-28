# Project Title

## Description
This project is a log visualization dashboard that allows users to view, filter, and analyze log data in real-time. It aims to provide insights into log activity and trends, enhancing decision-making and troubleshooting processes.

## Features
- **Real-time Data Processing**: Allows users to receive updates in real-time, addressing the need for timely information in applications such as chat systems or live dashboards.
- **Data Visualization**: Offers graphical representations of log data, making it easier for users to understand complex information and trends.
- **API Integration**: Connects with a backend service to fetch and send log data, enabling a comprehensive user experience.
- **Responsive Design**: Ensures that the application is accessible on various devices, solving the issue of usability across different screen sizes.

## Technologies Used
This project is built using the following technologies:
- **Backend**: C++ with Crow framework
- **Frontend**: React
- **Database**: SQLite
- **Other Tools**:Git, Recharts (for data visualization)

## Installation

### Backend
1. **Clone the repository**:
   ```bash
   git clone https://github.com/sauravjaiswalsj/LogVisualizer.git
   cd LogVisualizer/backend
   ```

2. **Build the backend**:
   Make sure you have a C++ compiler installed (like g++) and Crow library set up. You can compile the backend using:
   ```bash
   g++ -o main src/main.cpp -I/path/to/crow/include -I/path/to/nlohmann/json/include
   ```

3. **Run the backend**:
   ```bash
   ./main
   ```

### Frontend
1. **Navigate to the frontend directory**:
   ```bash
   cd ../log-visualization-frontend
   ```

2. **Install dependencies**:
   Make sure you have Node.js and npm installed. Then run:
   ```bash
   npm install
   ```

3. **Build the frontend**:
   ```bash
   npm run build
   ```

4. **Run the frontend**:
   ```bash
   npm start
   ```

## Usage
- Access the frontend application at `http://localhost:3000` (or the port specified in your React app).
- The backend API will be running at `http://localhost:8080/api/logs`.

## Contributing
Contributions are welcome! Please follow these steps to contribute:
1. Fork the repository.
2. Create a new branch (`git checkout -b feature/YourFeature`).
3. Make your changes and commit them (`git commit -m 'Add some feature'`).
4. Push to the branch (`git push origin feature/YourFeature`).
5. Open a pull request.

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments
- [Any resources, libraries, or individuals you want to acknowledge]
