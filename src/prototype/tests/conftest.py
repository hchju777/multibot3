"""테스트가 상위 디렉터리의 시제 모듈을 임포트할 수 있게 한다."""

import os
import sys

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
