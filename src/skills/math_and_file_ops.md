# Skill: Math Computation & Workspace File Operations
Keywords: math, calculate, expression, file, write, read, append, count, save, result

## Mục đích
Hướng dẫn Agent xử lý chính xác các bài toán số học kết hợp với các thao tác đọc, ghi và lưu trữ tệp tin trong môi trường làm việc (`workspace`).

## Quy tắc sử dụng Tool
1. **Quy tắc công cụ Calculator (`calculator`):**
   - Chỉ truyền chuỗi biểu thức số học thuần túy vào tham số `expression`.
   - Ví dụ: `{"tool": "calculator", "args": {"expression": "(450 * 12) + 850"}}`
   - Không đưa văn bản tự nhiên vào trong chuỗi `expression`.

2. **Quy tắc công cụ File (`file`):**
   - **Ghi tệp:** `{"action": "write", "path": "<tên_file>", "content": "<nội dung>"}`
   - **Đọc tệp:** `{"action": "read", "path": "<tên_file>"}`
   - **Nối tệp:** `{"action": "append", "path": "<tên_file>", "content": "<nội dung>"}`
   - Lưu ý: Đường dẫn tệp (`path`) phải là đường dẫn tương đối trong workspace, ví dụ `result.txt`, không dùng đường dẫn tuyệt đối root (`/`).

3. **Ví dụ kết hợp Math + File:**
   - *Task:* "Tính diện tích hình chữ nhật kích thước 25 và 40 rồi lưu vào area.txt"
   - *Thought:* Cần tính 25 * 40 trước bằng calculator.
   - *Action:* tool_call
   - *Action Input:* {"tool": "calculator", "args": {"expression": "25 * 40"}}
   - *(Observation: 1000)*
   - *Thought:* Kết quả là 1000, bây giờ ghi kết quả này vào area.txt.
   - *Action:* tool_call
   - *Action Input:* {"tool": "file", "args": {"action": "write", "path": "area.txt", "content": "1000"}}
   - *(Observation: Success)*
   - *Action:* final_answer
   - *Action Input:* Diện tích hình chữ nhật là 1000 và đã được lưu vào file area.txt.