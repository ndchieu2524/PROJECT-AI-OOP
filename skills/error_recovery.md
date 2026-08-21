# Skill: Tool Error Recovery & Self-Correction
Keywords: error, failed, not found, fix, retry, syntax, exception, timeout, invalid

## Mục đích
Hướng dẫn Agent tự động xử lý và phục hồi khi gặp lỗi thực thi công cụ (Tool Execution Failure), cú pháp JSON không hợp lệ, hoặc tham số sai định dạng.

## Quy tắc xử lý lỗi (Guidelines)
1. **Phân loại lỗi từ Observation:**
   - **Lỗi thiếu tham số (Missing Parameter):** Kiểm tra lại schema của Tool (ví dụ: `file` bắt buộc phải có `"action": "write"` hoặc `"action": "read"`).
   - **Lỗi file không tồn tại (File Not Found):** Kiểm tra xem file đã được tạo ở bước trước chưa, hoặc dùng `exec` để kiểm tra thư mục hiện tại (`ls` / `dir`).
   - **Lỗi biểu thức tính toán (Math Syntax Error):** Kiểm tra dấu ngoặc, phép toán trước khi gọi lại `calculator`.

2. **Chiến lược phục hồi (Recovery Strategy):**
   - **Không lặp lại nguyên văn lệnh lỗi:** Nếu một action bị lỗi, tuyệt đối không gửi lại JSON giống hệt ở bước tiếp theo để tránh kích hoạt `LoopDetector`.
   - **Điều chỉnh tham số:** Sửa lại tên trường (`args`) hoặc giá trị bị thiếu dựa trên thông báo lỗi trả về.
   - **Dừng lại nếu bất khả thi:** Nếu thử lại 2 lần với các cách khác nhau vẫn lỗi, chuyển sang `final_answer` giải thích rõ nguyên nhân thay vì tiếp tục gọi tool lặp đi lặp lại.

3. **Ví dụ xử lý lỗi:**
   - *Observation:* `{"success": false, "error": "Missing mandatory parameter: action"}`
   - *Thought:* Tool báo lỗi thiếu trường 'action'. Tôi cần thêm '"action": "write"' vào JSON args.
   - *Action:* tool_call
   - *Action Input:* {"tool": "file", "args": {"action": "write", "path": "output.txt", "content": "12345"}}